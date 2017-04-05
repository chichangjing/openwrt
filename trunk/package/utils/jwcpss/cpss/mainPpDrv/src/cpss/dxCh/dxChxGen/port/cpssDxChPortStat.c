/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortStat.c
*
* DESCRIPTION:
*       CPSS implementation for core port statistics and
*       egress counters facility.
*
* FILE REVISION NUMBER:
*       $Revision: 58 $
*******************************************************************************/

/* macro needed to support the call to PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC */
/* this define must come before include files */
#define PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_SUPPORTED_FLAG_CNS
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortLog.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortStat.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/dxCh/dxChxGen/cos/private/prvCpssDxChCoS.h>
#include <cpssCommon/private/prvCpssMath.h>
#include <cpss/dxCh/dxChxGen/port/PortMapping/prvCpssDxChPortMappingShadowDB.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/macCtrl/prvCpssDxChMacCtrl.h>

#ifdef PRESTERA_DEBUG
#define PORT_DEBUG
#endif /* PRESTERA_DEBUG */

#ifdef PORT_DEBUG
#define DBG_INFO(x) osPrintf x
#else
#define DBG_INFO(x)
#endif

/* does this port uses the GE port mechanism or the XG mechanism */
/* 1 - port uses GE port mechanism */
/* 0 - port uses XG port mechanism */
#define IS_GE_PORT_COUNTERS_MAC(_devNum,_portNum)                    \
    ((PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(_devNum,_portNum) == GT_FALSE &&    \
      PRV_CPSS_DXCH_PORT_TYPE_MAC(_devNum,_portNum) <= PRV_CPSS_PORT_GE_E) ? \
      ((_portNum < 24) ? 1 : 0) : 0)

/* does this port need to emulate old MIB counters behavior */
#define IS_MIB_API_SW_EMULATED_MAC(_devNum,_portNum)                              \
     (((PRV_CPSS_PP_MAC(_devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) &&     \
       (PRV_CPSS_DXCH_PORT_TYPE_MAC(_devNum,_portNum) == PRV_CPSS_PORT_XLG_E)) || \
      (((PRV_CPSS_PP_MAC(_devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||   \
        (PRV_CPSS_SIP_5_CHECK_MAC(devNum)) || \
        (PRV_CPSS_PP_MAC(_devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)) &&  \
        ((_portNum != CPSS_CPU_PORT_NUM_CNS) || (PRV_CPSS_DXCH_PP_MAC(_devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_TRUE))))

/******* info about Ethernet CPU port ******************************************
    support only next counters:

    1. CPSS_GOOD_PKTS_SENT_E:      32 bits counter
    2. CPSS_MAC_TRANSMIT_ERR_E:    16 bits counter
    3. CPSS_GOOD_OCTETS_SENT_E:    32 bits counter
    4. CPSS_DROP_EVENTS_E:         16 bits counter
    5. CPSS_GOOD_PKTS_RCV_E:       16 bits counter
    6. CPSS_BAD_PKTS_RCV_E:        16 bits counter
    7. CPSS_GOOD_OCTETS_RCV_E:     32 bits counter
    8. CPSS_BAD_OCTETS_RCV_E:      16 bits counter

*******************************************************************************/

/* array of MAC counters offsets */
/* gtMacCounterOffset[0] - offsets for not XGMII interface */
static GT_U8 gtMacCounterOffset[CPSS_LAST_MAC_COUNTER_NUM_E];

/*******************************************************************************
* internal_cpssDxChPortStatInit
*
* DESCRIPTION:
*       Init port statistics counter set CPSS facility.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortStatInit
(
    IN  GT_U8       devNum
)
{
    GT_U8  i;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    gtMacCounterOffset[CPSS_GOOD_OCTETS_RCV_E]  = 0x0;
    gtMacCounterOffset[CPSS_BAD_OCTETS_RCV_E]   = 0x8;
    gtMacCounterOffset[CPSS_GOOD_OCTETS_SENT_E] = 0x38;
    gtMacCounterOffset[CPSS_GOOD_UC_PKTS_RCV_E] = 0x10;
    gtMacCounterOffset[CPSS_GOOD_UC_PKTS_SENT_E] = 0x40;
    gtMacCounterOffset[CPSS_MULTIPLE_PKTS_SENT_E] = 0x50;
    gtMacCounterOffset[CPSS_DEFERRED_PKTS_SENT_E] = 0x14;

    for (i = CPSS_MAC_TRANSMIT_ERR_E; i <= CPSS_GOOD_OCTETS_SENT_E; i++)
        gtMacCounterOffset[i] = (GT_U8)(0x4 + (i * 4));

    for (i = CPSS_GOOD_OCTETS_SENT_E + 1; i < CPSS_GOOD_UC_PKTS_RCV_E; i++)
        gtMacCounterOffset[i] = (GT_U8)(0x8 + (i * 4));

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortStatInit
*
* DESCRIPTION:
*       Init port statistics counter set CPSS facility.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortStatInit
(
    IN  GT_U8       devNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortStatInit);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum));

    rc = internal_cpssDxChPortStatInit(devNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* dxChMacCountersCpuPortGet
*
* DESCRIPTION:
*       Gets Ethernet CPU port MAC counter .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*                  CPU port counters are valid only when using "Ethernet CPU
*                  port" (not using the SDMA interface).
*                  when using the SDMA interface refer to the API
*                  cpssDxChNetIfSdmaRxCountersGet(...)
*
* INPUTS:
*       devNum   - physical device number
*       cntrName - specific counter name
*
* OUTPUTS:
*       cntrValuePtr - (pointer to) current counter value.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_NOT_SUPPORTED - not supported counter on the port
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS dxChMacCountersCpuPortGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_PORT_MAC_COUNTERS_ENT  cntrName,
    OUT GT_U64                      *cntrValuePtr
)
{
    GT_U32  regAddr = /*0x00000060*/
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.macCounters;

    switch(cntrName)
    {
        case CPSS_GOOD_PKTS_SENT_E:
            regAddr += 0x0; /* 32 bits */
            break;
        case CPSS_MAC_TRANSMIT_ERR_E:
            regAddr += 0x4; /* 16 bits */
            break;
        case CPSS_GOOD_OCTETS_SENT_E:
            regAddr += 0x8; /* 32 bits */
            break;
        case CPSS_DROP_EVENTS_E:
            regAddr += 0xC; /* 16 bits */
            break;
        case CPSS_GOOD_PKTS_RCV_E:
            regAddr += 0x10;/* 16 bits */
            break;
        case CPSS_BAD_PKTS_RCV_E:
            regAddr += 0x14;/* 16 bits */
            break;
        case CPSS_GOOD_OCTETS_RCV_E:
            regAddr += 0x18;/* 32 bits */
            break;
        case CPSS_BAD_OCTETS_RCV_E:
            regAddr += 0x1c;/* 16 bits */
            break;
        default:
            /* not supported counter */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    return prvCpssDrvHwPpPortGroupReadRegister(devNum, PRV_CPSS_CPU_PORT_PORT_GROUP_ID_MAC(devNum),
            regAddr, &(cntrValuePtr->l[0]));
}
/*******************************************************************************
* prvCpssDxChPortSumMibEntry
*
* DESCRIPTION:
*       Sums the counters (destEntry+addEntryPtr), puts the result to destEntry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       destEntryArr
*       addEntryArr
*
* OUTPUTS:
*       destEntryArr
*
* RETURNS:
*       none
*
* COMMENTS:
*       none
*
*******************************************************************************/
static GT_VOID prvCpssDxChPortSumMibEntry
(
    INOUT GT_U32 destEntryArr[PRV_CPSS_XLG_MIB_COUNTERS_ENTRY_SIZE_CNS],
    IN    GT_U32 addEntryArr[PRV_CPSS_XLG_MIB_COUNTERS_ENTRY_SIZE_CNS]
)
{
    GT_U32 i;
    GT_U64 temp1, temp2;
    for (i = 0 ; i < PRV_CPSS_XLG_MIB_COUNTERS_ENTRY_SIZE_CNS ; i++)
    {
        if (i == 0 || i == 14)
        {
            temp1.l[0] = destEntryArr[i];
            temp1.l[1] = destEntryArr[i+1];
            temp2.l[0] = addEntryArr[i];
            temp2.l[1] = addEntryArr[i+1];
            temp1 = prvCpssMathAdd64(temp1,temp2);
            destEntryArr[i] = temp1.l[0];
            destEntryArr[i+1] = temp1.l[1];
            i++;
        }
        else
        {
            destEntryArr[i] += addEntryArr[i];
        }
    }
}


/*******************************************************************************
* prvCpssDxChMacCounterGet
*
* DESCRIPTION:
*       Gets Ethernet MAC counter /  MAC Captured counter for a
*       specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - physical device number
*       portNum        - physical port number,
*                        CPU port if getFromCapture is GT_FALSE
*       cntrName       - specific counter name
*       getFromCapture -  GT_TRUE -  Gets the captured Ethernet MAC counter
*                         GT_FALSE - Gets the Ethernet MAC counter
*
* OUTPUTS:
*       cntrValuePtr - (pointer to) current counter value.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number, device or counter name
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     The 10G MAC MIB counters are 64-bit wide.
*     Not supported counters: CPSS_BAD_PKTS_RCV_E, CPSS_UNRECOG_MAC_CNTR_RCV_E,
*     CPSS_BadFC_RCV_E, CPSS_GOOD_PKTS_RCV_E, CPSS_GOOD_PKTS_SENT_E.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChMacCounterGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  CPSS_PORT_MAC_COUNTERS_ENT  cntrName,
    IN  GT_BOOL                     getFromCapture,
    OUT GT_U64                      *cntrValuePtr
)
{
    GT_U32 regAddr;         /* register address */
    GT_U32 baseRegAddr;     /* base register address */
    GT_U32 regVal;        /* register value */
    GT_U32 portGroupId;/*the port group Id - support multi-port-groups device */
    GT_STATUS rc;      /* return code */
    GT_U32 *entryPtr;
    GT_U32 xlgMibEntry[PRV_CPSS_XLG_MIB_COUNTERS_ENTRY_SIZE_CNS];
    PRV_CPSS_DXCH_PORT_XLG_MIB_SHADOW_STC *xlgMibShadowPtr;
    GT_U32 portMacNum; /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);



    if(getFromCapture == GT_TRUE)
    {
        PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    }
    else
    {
        PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);

    CPSS_NULL_PTR_CHECK_MAC(cntrValuePtr);

    if((GT_U32)cntrName >= (GT_U32)CPSS_LAST_MAC_COUNTER_NUM_E)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    cntrValuePtr->l[0] = 0;
    cntrValuePtr->l[1] = 0;

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_FALSE &&
        portMacNum == CPSS_CPU_PORT_NUM_CNS)
    {
        /* the CPU port support only 8 counters */
        return dxChMacCountersCpuPortGet(devNum,cntrName,cntrValuePtr);
    }

    /* these counters do NOT exist */
    if ( (cntrName == CPSS_BAD_PKTS_RCV_E) ||
         (cntrName == CPSS_UNRECOG_MAC_CNTR_RCV_E) ||
         (cntrName == CPSS_BadFC_RCV_E) ||
         (cntrName == CPSS_GOOD_PKTS_RCV_E) ||
         (cntrName == CPSS_GOOD_PKTS_SENT_E))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* CPSS_COLLISIONS_E is the PFC counter.
           CPSS_LATE_COLLISIONS_E is the Unknown FC counter.
           The following counters do NOT exist for 10/20/40 Giga ports in eArch devices. */
        if( (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) >= PRV_CPSS_PORT_XG_E) &&
            ( (cntrName == CPSS_EXCESSIVE_COLLISIONS_E) ||
              (cntrName == CPSS_MULTIPLE_PKTS_SENT_E) ||
              (cntrName == CPSS_DEFERRED_PKTS_SENT_E)))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        /* these counters do NOT exist for 10/20/40 Giga ports */
        if( (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) >= PRV_CPSS_PORT_XG_E) &&
            ( (cntrName == CPSS_EXCESSIVE_COLLISIONS_E) ||
              (cntrName == CPSS_COLLISIONS_E) ||
              (cntrName == CPSS_LATE_COLLISIONS_E) ||
              (cntrName == CPSS_MULTIPLE_PKTS_SENT_E) ||
              (cntrName == CPSS_DEFERRED_PKTS_SENT_E)))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    /* XLG/MSM MIB counters don't support single counter read, clear on read configuration */
    /* and capture. We emulate this features using shadow MIB entries. */
    if(IS_MIB_API_SW_EMULATED_MAC(devNum,portMacNum))
    {
        xlgMibShadowPtr = PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMibShadowArr[portMacNum];
        if(getFromCapture == GT_TRUE)
        {
            entryPtr = xlgMibShadowPtr->captureMibShadow;
        }
        else
        {
            /* read the whole entry */
            rc = prvCpssDrvHwPpPortGroupReadRam(devNum,portGroupId,
                  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portMacNum].macCounters,
                  PRV_CPSS_XLG_MIB_COUNTERS_ENTRY_SIZE_CNS,xlgMibEntry);
            if (rc != GT_OK)
            {
                return rc;
            }
            /* add to shadow */
            prvCpssDxChPortSumMibEntry(xlgMibShadowPtr->mibShadow,xlgMibEntry);
            entryPtr = xlgMibShadowPtr->mibShadow;
        }

        cntrValuePtr->l[0] = entryPtr[gtMacCounterOffset[cntrName]/4];

        if(getFromCapture == GT_FALSE && xlgMibShadowPtr->clearOnReadEnable == GT_TRUE)
        {
            entryPtr[gtMacCounterOffset[cntrName]/4] = 0;
        }

        if (cntrName == CPSS_GOOD_OCTETS_RCV_E || cntrName == CPSS_GOOD_OCTETS_SENT_E)
        {
            cntrValuePtr->l[1] = entryPtr[gtMacCounterOffset[cntrName]/4 +1];
            if(getFromCapture == GT_FALSE && xlgMibShadowPtr->clearOnReadEnable == GT_TRUE)
            {
                entryPtr[gtMacCounterOffset[cntrName]/4 +1] = 0;
            }
        }

        return GT_OK;
    }

    if(getFromCapture == GT_TRUE)
    {
        baseRegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                  perPortRegs[portMacNum].macCaptureCounters;
    }
    else
    {
        baseRegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                  perPortRegs[portMacNum].macCounters;
    }

    regAddr = baseRegAddr + gtMacCounterOffset[cntrName];

    if ( prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,
            regAddr, &(cntrValuePtr->l[0])) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);


    switch (cntrName)
    {
        case CPSS_GOOD_OCTETS_RCV_E:
        case CPSS_GOOD_OCTETS_SENT_E:
            /* this counter has 64 bits */
            regAddr = regAddr + 4;
            if ( prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,
                    regAddr, &(cntrValuePtr->l[1])) != GT_OK)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            break;

        case CPSS_OVERSIZE_PKTS_E:
            /* Wrong read value for oversize and jabber counters in XG MIBs
               (FEr#85)*/
            if((PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_XG_MIB_READ_OVERSIZE_AND_JABBER_COUNTERS_WA_E) ==
                GT_TRUE) && (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) >= PRV_CPSS_PORT_XG_E))
            {
                /* Dummy read of GT_JABBER_PKTS */
                regAddr = baseRegAddr + gtMacCounterOffset[cntrName + 1];

                if (prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,
                        regAddr, &regVal) != GT_OK)
                   CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }
            break;
        case CPSS_JABBER_PKTS_E:
            if((PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,PRV_CPSS_DXCH_XG_MIB_READ_OVERSIZE_AND_JABBER_COUNTERS_WA_E) ==
                GT_TRUE) && (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) >= PRV_CPSS_PORT_XG_E))
            {
                /* Last read was GT_OVERSIZE_PKTS. Read again. */
                if (prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,
                        regAddr, &(cntrValuePtr->l[0])) != GT_OK)
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }
            break;
        default:
            break;
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChMacCounterGet
*
* DESCRIPTION:
*       Gets Ethernet MAC counter for a specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number or CPU port
*       cntrName - specific counter name
*
* OUTPUTS:
*       cntrValuePtr - (pointer to) current counter value.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number, device or counter name
*       GT_HW_ERROR  - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     1. The following counters are not supported:
*        Tri-Speed Ports:
*          CPSS_BAD_PKTS_RCV_E, CPSS_UNRECOG_MAC_CNTR_RCV_E, CPSS_BadFC_RCV_E,
*          CPSS_GOOD_PKTS_RCV_E and CPSS_GOOD_PKTS_SENT_E.
*        XG / HyperG.Stack / XLG Potrs:
*          CPSS_BAD_PKTS_RCV_E, CPSS_UNRECOG_MAC_CNTR_RCV_E, CPSS_BadFC_RCV_E,
*          CPSS_GOOD_PKTS_RCV_E, CPSS_GOOD_PKTS_SENT_E,
*          CPSS_EXCESSIVE_COLLISIONS_E, CPSS_COLLISIONS_E,
*          CPSS_LATE_COLLISIONS_E and CPSS_DEFERRED_PKTS_SENT_E.
*     2. CPU port counters are valid only when using "Ethernet CPU port", i.e.
*          not using SDMA interface.
*        When using SDMA interface the following APIs are relevant:
*          cpssDxChNetIfSdmaRxCountersGet, cpssDxChNetIfSdmaRxErrorCountGet.
*     3. The following counters are supported for CPU port:
*           CPSS_GOOD_PKTS_SENT_E, CPSS_MAC_TRANSMIT_ERR_E, CPSS_DROP_EVENTS_E,
*           CPSS_GOOD_OCTETS_SENT_E, CPSS_GOOD_PKTS_RCV_E, CPSS_BAD_PKTS_RCV_E,
*           CPSS_GOOD_OCTETS_RCV_E, CPSS_BAD_OCTETS_RCV_E.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMacCounterGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  CPSS_PORT_MAC_COUNTERS_ENT  cntrName,
    OUT GT_U64                      *cntrValuePtr
)
{
    PRV_CPSS_DXCH_ERRATA_STACK_MAC_COUNT_SHADOW_STC *macCountShadowPtr; /* Pointer To Shadow of all
                                                    MAC Counters of ports 24-27 */
    GT_BOOL                                         clearOnReadWa;      /* clear On Read WA   */
    GT_U64                                          cntrValue;          /* counter value      */
    GT_U64                                          *cntrShadowPtr;     /* Pointer To Shadow of
                                                    MAC Counter specified by function parameters */
    GT_STATUS                                       rc;                 /* return code        */
    GT_U32 portMacNum; /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    CPSS_NULL_PTR_CHECK_MAC(cntrValuePtr);
    if((GT_U32)cntrName >= (GT_U32)CPSS_LAST_MAC_COUNTER_NUM_E)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    /* default */
    clearOnReadWa     = GT_FALSE;
    macCountShadowPtr = NULL;

    if (PRV_CPSS_DXCH_XCAT2_STACK_MAC_COUNT_NO_CLEAR_ON_READ_WA_MAC(devNum, portMacNum))
    {
        macCountShadowPtr =
        PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
            info_PRV_CPSS_DXCH_XCAT2_STACK_MAC_COUNT_NO_CLEAR_ON_READ_WA_E.
            stackMacCntPtr;
        clearOnReadWa =
            macCountShadowPtr->stackPortArr
            [portMacNum - PRV_CPSS_DXCH_ERRATA_STACK_PORT_BASE_CNS].clearOnRead;
    }

    rc = prvCpssDxChMacCounterGet(
        devNum, portNum, cntrName, GT_FALSE, &cntrValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (clearOnReadWa == GT_FALSE)
    {
        *cntrValuePtr = cntrValue;
        return GT_OK;
    }

    /* clearOnRead Work Around implementation */

    cntrShadowPtr =
        &(macCountShadowPtr->stackPortArr[portMacNum - 24].
          macCntrShadow[cntrName]);

    /* subtract the shadowed value instead of clearing the counter */
    *cntrValuePtr = prvCpssMathSub64(cntrValue, *cntrShadowPtr);

    /* update the counter shadow */
    *cntrShadowPtr = cntrValue;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChMacCounterGet
*
* DESCRIPTION:
*       Gets Ethernet MAC counter for a specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number or CPU port
*       cntrName - specific counter name
*
* OUTPUTS:
*       cntrValuePtr - (pointer to) current counter value.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number, device or counter name
*       GT_HW_ERROR  - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     1. The following counters are not supported:
*        Tri-Speed Ports:
*          CPSS_BAD_PKTS_RCV_E, CPSS_UNRECOG_MAC_CNTR_RCV_E, CPSS_BadFC_RCV_E,
*          CPSS_GOOD_PKTS_RCV_E and CPSS_GOOD_PKTS_SENT_E.
*        XG / HyperG.Stack / XLG Potrs:
*          CPSS_BAD_PKTS_RCV_E, CPSS_UNRECOG_MAC_CNTR_RCV_E, CPSS_BadFC_RCV_E,
*          CPSS_GOOD_PKTS_RCV_E, CPSS_GOOD_PKTS_SENT_E,
*          CPSS_EXCESSIVE_COLLISIONS_E, CPSS_COLLISIONS_E,
*          CPSS_LATE_COLLISIONS_E and CPSS_DEFERRED_PKTS_SENT_E.
*     2. CPU port counters are valid only when using "Ethernet CPU port", i.e.
*          not using SDMA interface.
*        When using SDMA interface the following APIs are relevant:
*          cpssDxChNetIfSdmaRxCountersGet, cpssDxChNetIfSdmaRxErrorCountGet.
*     3. The following counters are supported for CPU port:
*           CPSS_GOOD_PKTS_SENT_E, CPSS_MAC_TRANSMIT_ERR_E, CPSS_DROP_EVENTS_E,
*           CPSS_GOOD_OCTETS_SENT_E, CPSS_GOOD_PKTS_RCV_E, CPSS_BAD_PKTS_RCV_E,
*           CPSS_GOOD_OCTETS_RCV_E, CPSS_BAD_OCTETS_RCV_E.
*
*******************************************************************************/
GT_STATUS cpssDxChMacCounterGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  CPSS_PORT_MAC_COUNTERS_ENT  cntrName,
    OUT GT_U64                      *cntrValuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMacCounterGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, cntrName, cntrValuePtr));

    rc = internal_cpssDxChMacCounterGet(devNum, portNum, cntrName, cntrValuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, cntrName, cntrValuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortMacCountersIsSupportedCheck
*
* DESCRIPTION:
*       Checks if the counter is supported by current device and port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum               - physical device number
*       cntrName             - counter name
*       portMacNum           - port MAC number
*       counterMode          - CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_NORMAL_E -
*                                    MIB counter index 4 is sent deferred. MIB counter index 12 is Frames1024toMaxOctets
*                              CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_HISTOGRAM_1518_E -
*                                    MIB counter index 4 is Frames1024to1518Octets. MIB counter index 12 is Frames1519toMaxOctets
*
* OUTPUTS:
*
* RETURNS:
*       GT_TRUE       - if counter is supported by current device and port
*       GT_FALSE      - if counter is not supported by current device and port
*
* COMMENTS:
*
*******************************************************************************/
static GT_BOOL prvCpssDxChPortMacCountersIsSupportedCheck
(
    IN  GT_U8                                                  devNum,
    IN  GT_U32                                                 portMacNum,
    IN  CPSS_PORT_MAC_COUNTERS_ENT                             cntrName,
    IN  CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_ENT  counterMode
)
{
    GT_BOOL result = GT_TRUE;

    if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E) &&
           (portMacNum != CPSS_CPU_PORT_NUM_CNS))
    {
        if(((cntrName == CPSS_PKTS_1024TO1518_OCTETS_E) || (cntrName == CPSS_PKTS_1519TOMAX_OCTETS_E)) &&
            (CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_NORMAL_E == counterMode))
        {
            result = GT_FALSE;
        }
        if((cntrName == CPSS_PKTS_1024TOMAX_OCTETS_E) && (CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_NORMAL_E != counterMode))
        {
            result = GT_FALSE;
        }
    }
    return result;
}

/*******************************************************************************
* prvCpssDxChPortMacCountersInterlakenChannelGet
*
* DESCRIPTION:
*       Gets Ethernet MAC counters per Interlaken port channel.
*
* APPLICABLE DEVICES:
*        Bobcat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum         - physical device number
*       portNum        - physical port number associated with channel
*
* OUTPUTS:
*       portMacCounterSetArrayPtr - (pointer to) array of current counter values.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortMacCountersInterlakenChannelGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT CPSS_PORT_MAC_COUNTER_SET_STC   *portMacCounterSetArrayPtr
)
{
    GT_STATUS                   rc; /* return code */
    CPSS_PORT_MAC_COUNTERS_ENT  cntrName;                               /* read the counter name  */
    GT_U32                      ilknMibEntry[PRV_CPSS_ILKN_RX_MIB_COUNTERS_ENTRY_SIZE_CNS]; /* HW data */
    GT_U32                      portMacNum;  /* Port MAC number */
    GT_U32                      regAddr;     /* register address */
    GT_U32                      regValue;    /* register value */
    GT_U32                      portGroupId; /*the port group Id - support multi-port-groups device */
    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr; /* pointer to mapping shadow DB */

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);

    /* Get detailed info about ILKN port */
    rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum, portNum, &portMapShadowPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    regValue = portMapShadowPtr->portMap.ilknChannel;
    /* Copy channel data to shadow, by configuring <channel select> register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->GOP.PR.PRMIB.MIBChannelSelect;
    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, 0, 6, regValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->GOP.PR.PRMIB.MIBCountersBase;

    /* read the whole entry */
    rc = prvCpssDrvHwPpPortGroupReadRam(devNum,portGroupId,
          regAddr, PRV_CPSS_ILKN_RX_MIB_COUNTERS_ENTRY_SIZE_CNS,ilknMibEntry);
    if (rc != GT_OK)
    {
        return rc;
    }

    for (cntrName = 0 ; cntrName < CPSS_LAST_MAC_COUNTER_NUM_E ; cntrName++)
    {
        switch (cntrName)
        {
            case CPSS_GOOD_OCTETS_RCV_E:
                portMacCounterSetArrayPtr->goodOctetsRcv.l[0] = ilknMibEntry[0];
                portMacCounterSetArrayPtr->goodOctetsRcv.l[1] = ilknMibEntry[1];
                break;
            case CPSS_GOOD_UC_PKTS_RCV_E:
                portMacCounterSetArrayPtr->ucPktsRcv.l[0] = ilknMibEntry[2];
                break;
            case CPSS_BRDC_PKTS_RCV_E:
                portMacCounterSetArrayPtr->brdcPktsRcv.l[0] = ilknMibEntry[3];
                break;
            case CPSS_MC_PKTS_RCV_E:
                portMacCounterSetArrayPtr->mcPktsRcv.l[0] = ilknMibEntry[4];
                break;
            case CPSS_PKTS_64_OCTETS_E:
                portMacCounterSetArrayPtr->pkts64Octets.l[0] = ilknMibEntry[5];
                break;
            case CPSS_PKTS_65TO127_OCTETS_E:
                portMacCounterSetArrayPtr->pkts65to127Octets.l[0] = ilknMibEntry[6];
                break;
            case CPSS_PKTS_128TO255_OCTETS_E:
                portMacCounterSetArrayPtr->pkts128to255Octets.l[0] = ilknMibEntry[7];
                break;
            case CPSS_PKTS_256TO511_OCTETS_E:
                portMacCounterSetArrayPtr->pkts256to511Octets.l[0] = ilknMibEntry[8];
                break;
            case CPSS_PKTS_512TO1023_OCTETS_E:
                portMacCounterSetArrayPtr->pkts512to1023Octets.l[0] = ilknMibEntry[9];
                break;
            case CPSS_PKTS_1024TO1518_OCTETS_E:
                portMacCounterSetArrayPtr->pkts1024to1518Octets.l[0] = ilknMibEntry[10];
                break;
            case CPSS_PKTS_1519TOMAX_OCTETS_E:
                portMacCounterSetArrayPtr->pkts1519toMaxOctets.l[0] = ilknMibEntry[11];
                break;
            case CPSS_DROP_EVENTS_E:
                portMacCounterSetArrayPtr->dropEvents.l[0] = ilknMibEntry[12];
                break;
            case CPSS_UNDERSIZE_PKTS_E:
                portMacCounterSetArrayPtr->undersizePkts.l[0] = ilknMibEntry[13];
                break;
            case CPSS_OVERSIZE_PKTS_E:
                portMacCounterSetArrayPtr->oversizePkts.l[0] = ilknMibEntry[14];
                break;
            case CPSS_MAC_RCV_ERROR_E:
                portMacCounterSetArrayPtr->macRcvError.l[0] = ilknMibEntry[15];
                break;

            case CPSS_BAD_OCTETS_RCV_E:
            case CPSS_MAC_TRANSMIT_ERR_E:
            case CPSS_GOOD_PKTS_RCV_E:
            case CPSS_PKTS_1024TOMAX_OCTETS_E:
            case CPSS_GOOD_OCTETS_SENT_E:
            case CPSS_GOOD_PKTS_SENT_E:
            case CPSS_EXCESSIVE_COLLISIONS_E:
            case CPSS_MC_PKTS_SENT_E:
            case CPSS_BRDC_PKTS_SENT_E:
            case CPSS_FC_SENT_E:
            case CPSS_GOOD_FC_RCV_E:
            case CPSS_FRAGMENTS_PKTS_E:
            case CPSS_JABBER_PKTS_E:
            case CPSS_BAD_CRC_E:
            case CPSS_COLLISIONS_E:
            case CPSS_LATE_COLLISIONS_E:
            case CPSS_GOOD_UC_PKTS_SENT_E:
            case CPSS_MULTIPLE_PKTS_SENT_E:
            case CPSS_DEFERRED_PKTS_SENT_E:
            case CPSS_BAD_PKTS_RCV_E:
            default:
                continue;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortMacCountersOnPortGet
*
* DESCRIPTION:
*       Gets Ethernet MAC counter for a particular Port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - physical device number
*       portNum        - physical port number,
*                        CPU port if getFromCapture is GT_FALSE
*       getFromCapture  -  GT_TRUE -  Gets the captured Ethernet MAC counter
*                         GT_FALSE - Gets the Ethernet MAC counter
*       readMode       - read counters procedure mode
*
* OUTPUTS:
*       portMacCounterSetArrayPtr - (pointer to) array of current counter values.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     Not supported counters: CPSS_BAD_PKTS_RCV_E, CPSS_UNRECOG_MAC_CNTR_RCV_E,
*     CPSS_BadFC_RCV_E, CPSS_GOOD_PKTS_RCV_E, CPSS_GOOD_PKTS_SENT_E.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortMacCountersOnPortGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  GT_BOOL                         getFromCapture,
    OUT CPSS_PORT_MAC_COUNTER_SET_STC   *portMacCounterSetArrayPtr,
    IN  PRV_DXCH_PORT_MAC_CNTR_READ_MODE_ENT readMode
)
{
    GT_U64                     cntrValue[CPSS_LAST_MAC_COUNTER_NUM_E];
    CPSS_PORT_MAC_COUNTERS_ENT cntrName;  /* read the counter name  */
    GT_STATUS                  rc;
    GT_U32  xlgMibEntry[PRV_CPSS_XLG_MIB_COUNTERS_ENTRY_SIZE_CNS];
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_DXCH_PORT_XLG_MIB_SHADOW_STC *xlgMibShadowPtr;
    GT_U32  portMacNum;
    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr; /* pointer to mapping shadow DB */
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue = 0;    /* HW value */
    GT_U32  regValue1 = 0;    /* HW value */
    CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_ENT  counterMode;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(getFromCapture == GT_TRUE)
    {
        PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    }
    else
    {
        PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    }

    if (readMode == PRV_DXCH_PORT_MAC_CNTR_READ_MODE_USUAL_E)
    {
        CPSS_NULL_PTR_CHECK_MAC(portMacCounterSetArrayPtr);
        cpssOsMemSet(portMacCounterSetArrayPtr, 0, sizeof(CPSS_PORT_MAC_COUNTER_SET_STC));
    }

    /* reset counter value */
    cpssOsMemSet(cntrValue, 0, sizeof(GT_U64)*CPSS_LAST_MAC_COUNTER_NUM_E);

    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);
    xlgMibShadowPtr = PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMibShadowArr[portMacNum];

    /***************************/
    /* INTERLAKEN MIB Counters */
    /***************************/
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* Get detailed info about ILKN port */
        rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum, portNum, &portMapShadowPtr);
        if(rc != GT_OK)
        {
            return rc;
        }

        if((portMapShadowPtr->valid == GT_TRUE) && (portMapShadowPtr->portMap.mappingType == CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E))
        {
            if (readMode != PRV_DXCH_PORT_MAC_CNTR_READ_MODE_USUAL_E)
            {
                /* ILNK channels do not need Ethernet MAC related WA. */
                return GT_OK;
            }

            rc = prvCpssDxChPortMacCountersInterlakenChannelGet(devNum, portNum, portMacCounterSetArrayPtr);
            return rc;
        }
    }

    /* XLG/MSM MIB counters don't support single counter read, clear on read configuration */
    /* and capture. We emulate this features using shadow MIB entries. */
    if(IS_MIB_API_SW_EMULATED_MAC(devNum,portMacNum))
    {
        if(getFromCapture == GT_TRUE)
        {
            cpssOsMemCpy(xlgMibEntry,xlgMibShadowPtr->captureMibShadow,sizeof(xlgMibEntry));
        }
        else
        {
            /* read the whole entry */
            rc = prvCpssDrvHwPpPortGroupReadRam(devNum,portGroupId,
                  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portMacNum].macCounters,
                  PRV_CPSS_XLG_MIB_COUNTERS_ENTRY_SIZE_CNS,xlgMibEntry);
            if (rc != GT_OK)
            {
                return rc;
            }

            if (readMode == PRV_DXCH_PORT_MAC_CNTR_READ_MODE_RESET_HW_E)
            {
                if(CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
                {
                    GT_BOOL                 enable;
                    CPSS_DXCH_PORT_COMBO_PARAMS_STC comboParams;

                    rc = cpssDxChPortComboModeEnableGet(devNum, portNum, &enable, &comboParams);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }

                    if(enable)
                    {
                        GT_U32  localPort;
                        GT_U32  cntrsAddr;
                        GT_U32  i;

                        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portMacNum);
                        for(i = 0; i < CPSS_DXCH_PORT_MAX_MACS_IN_COMBO_CNS; i++)
                        {
                            if(CPSS_DXCH_PORT_COMBO_NA_MAC_CNS == comboParams.macArray[i].macNum)
                                break;

                            if((localPort == comboParams.macArray[i].macNum) &&
                                (portGroupId == comboParams.macArray[i].macPortGroupNum))
                            {
                                continue;
                            }

                            cntrsAddr = (PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portMacNum].macCounters
                                            & 0xF000000) + (comboParams.macArray[i].macNum * 0x20000);
                            rc = prvCpssDrvHwPpPortGroupReadRam(devNum,
                                        comboParams.macArray[i].macPortGroupNum,
                                        cntrsAddr,
                                        PRV_CPSS_XLG_MIB_COUNTERS_ENTRY_SIZE_CNS,
                                        xlgMibEntry);
                            if (rc != GT_OK)
                            {
                                return rc;
                            }

                        }/* for(i = 0; */

                    }/* if combo port */

                }/* if lion2 */

                return GT_OK;
            }
            prvCpssDxChPortSumMibEntry(xlgMibEntry,xlgMibShadowPtr->mibShadow);
        }
    }

    if (readMode == PRV_DXCH_PORT_MAC_CNTR_READ_MODE_USUAL_E)
    {
        if (GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_BOBCAT2_FC_SENT_COUNTERS_PROBLEM_WHEN_FCU_BYPASS_DISABLE_WA_E))
        {
            PRV_CPSS_DXCH_PORT_FCA_REGS_ADDR_STC *fcaStcPtr;

            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
            {
                fcaStcPtr = (portMacNum == CPSS_CPU_PORT_NUM_CNS) ?
                     &PRV_DXCH_REG_UNIT_GOP_CPU_FCA_MAC(devNum) :
                     &PRV_DXCH_REG_UNIT_GOP_FCA_MAC(devNum, portMacNum);

                regAddr = fcaStcPtr->sentPktsCntrLSb;
            }
            else
            {
                regAddr = PRV_DXCH_REG1_UNIT_GOP_FCA_MAC(devNum, portMacNum).sentPktsCntrLSb;
            }
            rc =  prvCpssHwPpGetRegField(devNum, regAddr, 0, 16, &regValue);
            if (rc != GT_OK)
            {
                return rc;
            }

            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
            {
                fcaStcPtr = (portMacNum == CPSS_CPU_PORT_NUM_CNS) ?
                     &PRV_DXCH_REG_UNIT_GOP_CPU_FCA_MAC(devNum) :
                     &PRV_DXCH_REG_UNIT_GOP_FCA_MAC(devNum, portMacNum);

                regAddr = fcaStcPtr->sentPktsCntrMSb;
            }
            else
            {
                regAddr = PRV_DXCH_REG1_UNIT_GOP_FCA_MAC(devNum, portMacNum).sentPktsCntrMSb;
            }
            rc =  prvCpssHwPpGetRegField(devNum, regAddr, 0, 16, &regValue1);
            if (rc != GT_OK)
            {
                return rc;
            }

            regValue |= (regValue1 << 16);
        }

        /* get counterMode from register. The configuration does not exist for CPU MAC */
        if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E) &&
           (portMacNum != CPSS_CPU_PORT_NUM_CNS))
        {
            cpssDxChPortMacOversizedPacketsCounterModeGet(/*IN*/devNum,  /*IN*/portMacNum, /*OUT*/&counterMode);
        }

        /* get all mac counters */
        for(cntrName = 0; cntrName < CPSS_LAST_MAC_COUNTER_NUM_E; cntrName++)
        {
            if(!prvCpssDxChPortMacCountersIsSupportedCheck(/*IN*/devNum, /*IN*/portMacNum, /*IN*/cntrName, /*IN*/counterMode))
            {
                continue;
            }
            /* the CPU port support only 8 counters */
            if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_FALSE &&
                portMacNum == CPSS_CPU_PORT_NUM_CNS)
            {
                if ( (cntrName != CPSS_GOOD_PKTS_SENT_E) &&
                     (cntrName != CPSS_MAC_TRANSMIT_ERR_E) &&
                     (cntrName != CPSS_GOOD_OCTETS_SENT_E) &&
                     (cntrName != CPSS_DROP_EVENTS_E) &&
                     (cntrName != CPSS_GOOD_PKTS_RCV_E) &&
                     (cntrName != CPSS_BAD_PKTS_RCV_E) &&
                     (cntrName != CPSS_GOOD_OCTETS_RCV_E) &&
                     (cntrName != CPSS_BAD_OCTETS_RCV_E) )
                {
                    continue;
                }
            }
            else
            {
                /* these counters do NOT exist */
                if ( (cntrName == CPSS_BAD_PKTS_RCV_E) ||
                     (cntrName == CPSS_UNRECOG_MAC_CNTR_RCV_E) ||
                     (cntrName == CPSS_BadFC_RCV_E) ||
                     (cntrName == CPSS_GOOD_PKTS_RCV_E) ||
                     (cntrName == CPSS_GOOD_PKTS_SENT_E))
                {
                    continue;
                }
            }

            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                /* CPSS_COLLISIONS_E is the PFC counter.
                   CPSS_LATE_COLLISIONS_E is the Unknown FC counter.
                   The following counters do NOT exist for 10/20/40 Giga ports in eArch devices. */
                if( (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) >= PRV_CPSS_PORT_XG_E) &&
                    ( (cntrName == CPSS_EXCESSIVE_COLLISIONS_E) ||
                      (cntrName == CPSS_MULTIPLE_PKTS_SENT_E) ||
                      (cntrName == CPSS_DEFERRED_PKTS_SENT_E)))
                {
                    continue;
                }
            }
            else
            {
                /* these counters do NOT exist for 10/20/40 Giga ports */
                if( (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) >= PRV_CPSS_PORT_XG_E) &&
                    ( (cntrName == CPSS_EXCESSIVE_COLLISIONS_E) ||
                      (cntrName == CPSS_COLLISIONS_E) ||
                      (cntrName == CPSS_LATE_COLLISIONS_E) ||
                      (cntrName == CPSS_MULTIPLE_PKTS_SENT_E) ||
                      (cntrName == CPSS_DEFERRED_PKTS_SENT_E)))
                {
                    continue;
                }
            }

            if(IS_MIB_API_SW_EMULATED_MAC(devNum,portMacNum))
            {
                cntrValue[cntrName].l[0] = xlgMibEntry[gtMacCounterOffset[cntrName]/4];

                if ((cntrName == CPSS_GOOD_OCTETS_RCV_E) ||
                    (cntrName == CPSS_GOOD_OCTETS_SENT_E))
                {
                    cntrValue[cntrName].l[1] = xlgMibEntry[gtMacCounterOffset[cntrName]/4 + 1];
                }
            }
            else
            {
                /* get value for counter i */
                if(getFromCapture == GT_TRUE)
                {
                    rc = cpssDxChPortMacCounterCaptureGet(devNum, portNum, cntrName,
                                                          &cntrValue[cntrName]);
                }
                else
                {
                    rc = cpssDxChMacCounterGet(devNum, portNum, cntrName,
                                                &cntrValue[cntrName]);
                }
                if (rc != GT_OK)
                    return rc;
            }
        }

        for (cntrName = 0 ; cntrName < CPSS_LAST_MAC_COUNTER_NUM_E ; cntrName++)
        {
            if(!prvCpssDxChPortMacCountersIsSupportedCheck(/*IN*/devNum, /*IN*/portMacNum, /*IN*/cntrName, /*IN*/counterMode))
            {
                continue;
            }
            switch (cntrName)
            {
                case CPSS_GOOD_OCTETS_RCV_E:
                    portMacCounterSetArrayPtr->goodOctetsRcv = cntrValue[cntrName];
                    break;
                case CPSS_BAD_OCTETS_RCV_E:
                    portMacCounterSetArrayPtr->badOctetsRcv = cntrValue[cntrName];
                    break;
                case CPSS_MAC_TRANSMIT_ERR_E:
                    portMacCounterSetArrayPtr->macTransmitErr = cntrValue[cntrName];
                    break;
                /* This counter is for CPU port only */
                case CPSS_GOOD_PKTS_RCV_E:
                    portMacCounterSetArrayPtr->goodPktsRcv = cntrValue[cntrName];
                    break;
                case CPSS_BRDC_PKTS_RCV_E:
                    portMacCounterSetArrayPtr->brdcPktsRcv = cntrValue[cntrName];
                    break;
                case CPSS_MC_PKTS_RCV_E:
                    portMacCounterSetArrayPtr->mcPktsRcv = cntrValue[cntrName];
                    break;
                case CPSS_PKTS_64_OCTETS_E:
                    portMacCounterSetArrayPtr->pkts64Octets = cntrValue[cntrName];
                    break;
                case CPSS_PKTS_65TO127_OCTETS_E:
                    portMacCounterSetArrayPtr->pkts65to127Octets = cntrValue[cntrName];
                    break;
                case CPSS_PKTS_128TO255_OCTETS_E:
                    portMacCounterSetArrayPtr->pkts128to255Octets = cntrValue[cntrName];
                    break;
                case CPSS_PKTS_256TO511_OCTETS_E:
                    portMacCounterSetArrayPtr->pkts256to511Octets = cntrValue[cntrName];
                    break;
                case CPSS_PKTS_512TO1023_OCTETS_E:
                    portMacCounterSetArrayPtr->pkts512to1023Octets = cntrValue[cntrName];
                    break;
                case CPSS_PKTS_1024TOMAX_OCTETS_E:
                    portMacCounterSetArrayPtr->pkts1024tomaxOoctets = cntrValue[cntrName];
                    break;
                case CPSS_GOOD_OCTETS_SENT_E:
                    portMacCounterSetArrayPtr->goodOctetsSent = cntrValue[cntrName];
                    break;
                /* This counter is for CPU port only */
                case CPSS_GOOD_PKTS_SENT_E:
                    portMacCounterSetArrayPtr->goodPktsSent = cntrValue[cntrName];
                    break;
                case CPSS_EXCESSIVE_COLLISIONS_E:
                    portMacCounterSetArrayPtr->excessiveCollisions = cntrValue[cntrName];
                    break;
                case CPSS_MC_PKTS_SENT_E:
                    portMacCounterSetArrayPtr->mcPktsSent = cntrValue[cntrName];
                    break;
                case CPSS_BRDC_PKTS_SENT_E:
                    portMacCounterSetArrayPtr->brdcPktsSent = cntrValue[cntrName];
                    break;
                case CPSS_FC_SENT_E:
                    if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E) &&
                        (PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,portNum) == PRV_CPSS_GE_PORT_GE_ONLY_E))
                    {
                        portMacCounterSetArrayPtr->fcSent = cntrValue[cntrName];
                    }
                    else
                    {
                        if (GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_BOBCAT2_FC_SENT_COUNTERS_PROBLEM_WHEN_FCU_BYPASS_DISABLE_WA_E))
                        {
                            cntrValue[cntrName].l[0] += regValue;
                        }

                        portMacCounterSetArrayPtr->fcSent = cntrValue[cntrName];
                    }

                    break;
                case CPSS_GOOD_FC_RCV_E:
                    portMacCounterSetArrayPtr->goodFcRcv = cntrValue[cntrName];
                    break;
                case CPSS_DROP_EVENTS_E:
                    /* for Twist-D-XG, GT_DROP_EVENTS */
                    /*offset represent BadFCReceived */
                    portMacCounterSetArrayPtr->dropEvents = cntrValue[cntrName];
                    break;
                case CPSS_UNDERSIZE_PKTS_E:
                    portMacCounterSetArrayPtr->undersizePkts = cntrValue[cntrName];
                    break;
                case CPSS_FRAGMENTS_PKTS_E:
                    portMacCounterSetArrayPtr->fragmentsPkts = cntrValue[cntrName];
                    break;
                case CPSS_OVERSIZE_PKTS_E:
                    portMacCounterSetArrayPtr->oversizePkts = cntrValue[cntrName];
                    break;
                case CPSS_JABBER_PKTS_E:
                    portMacCounterSetArrayPtr->jabberPkts = cntrValue[cntrName];
                    break;
                case CPSS_MAC_RCV_ERROR_E:
                    portMacCounterSetArrayPtr->macRcvError = cntrValue[cntrName];
                    break;
                case CPSS_BAD_CRC_E:
                    portMacCounterSetArrayPtr->badCrc = cntrValue[cntrName];
                    break;
                case CPSS_COLLISIONS_E:
                    portMacCounterSetArrayPtr->collisions = cntrValue[cntrName];
                    break;
                case CPSS_LATE_COLLISIONS_E:
                    portMacCounterSetArrayPtr->lateCollisions = cntrValue[cntrName];
                    break;
                case CPSS_GOOD_UC_PKTS_RCV_E:
                    portMacCounterSetArrayPtr->ucPktsRcv = cntrValue[cntrName];
                    break;
                case CPSS_GOOD_UC_PKTS_SENT_E:
                    portMacCounterSetArrayPtr->ucPktsSent = cntrValue[cntrName];
                    break;
                case CPSS_MULTIPLE_PKTS_SENT_E:
                    portMacCounterSetArrayPtr->multiplePktsSent = cntrValue[cntrName];
                    break;
                case CPSS_DEFERRED_PKTS_SENT_E:
                    portMacCounterSetArrayPtr->deferredPktsSent = cntrValue[cntrName];
                    break;
                /* This counter is for CPU port only */
                case CPSS_BAD_PKTS_RCV_E:
                    portMacCounterSetArrayPtr->badPktsRcv = cntrValue[cntrName];
                    break;
                default:
                    break;

            }
        }
    }

    /* XLG/MSM MIB counters don't support single counter read, clear on read configuration */
    /* and capture. We emulate this features using shadow MIB entries. */
    if(IS_MIB_API_SW_EMULATED_MAC(devNum,portMacNum))
    {
        if ((xlgMibShadowPtr->clearOnReadEnable == GT_TRUE) &&
            (readMode != PRV_DXCH_PORT_MAC_CNTR_READ_MODE_UPDATE_SHADOW_E))
        {
            cpssOsMemSet(xlgMibShadowPtr->mibShadow, 0, sizeof(xlgMibShadowPtr->mibShadow));
        }
        else
        {
            cpssOsMemCpy(xlgMibShadowPtr->mibShadow,xlgMibEntry,sizeof(xlgMibShadowPtr->mibShadow));
        }
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortMacCountersSpecialShadowReset
*
* DESCRIPTION:
*       Reset Special Mib conters packet in Shadow to work arround hardware error.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number or CPU port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortMacCountersSpecialShadowReset
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum
)
{
    PRV_CPSS_DXCH_PORT_XLG_MIB_SHADOW_STC *xlgMibShadowPtr;
    GT_U32  portMacNum;
    CPSS_PORT_MAC_COUNTERS_ENT cntrName;  /* read the counter name  */
    GT_BOOL tobeReset = GT_TRUE;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    xlgMibShadowPtr = PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMibShadowArr[portMacNum];
    for (cntrName = 0 ; cntrName < CPSS_LAST_MAC_COUNTER_NUM_E ; cntrName++)
    {
            switch (cntrName)
            {
               case CPSS_PKTS_64_OCTETS_E:
                    if(xlgMibShadowPtr->mibShadow[gtMacCounterOffset[cntrName]/4] != (GT_U32)1)
                    {
                        tobeReset = GT_FALSE;
                    }
                    break;
                case CPSS_GOOD_OCTETS_SENT_E:
                    if(xlgMibShadowPtr->mibShadow[gtMacCounterOffset[cntrName]/4] != (GT_U32)64)
                    {
                        tobeReset = GT_FALSE;
                    }
                    break;
                case CPSS_MC_PKTS_SENT_E:
                    if(xlgMibShadowPtr->mibShadow[gtMacCounterOffset[cntrName]/4] != (GT_U32)1)
                    {
                        tobeReset = GT_FALSE;
                    }
                    break;
                case CPSS_GOOD_OCTETS_RCV_E:
                case CPSS_BAD_OCTETS_RCV_E:
                case CPSS_MAC_TRANSMIT_ERR_E:
                case CPSS_BRDC_PKTS_RCV_E:
                case CPSS_MC_PKTS_RCV_E:
                case CPSS_PKTS_65TO127_OCTETS_E:
                case CPSS_PKTS_128TO255_OCTETS_E:
                case CPSS_PKTS_256TO511_OCTETS_E:
                case CPSS_PKTS_512TO1023_OCTETS_E:
                case CPSS_PKTS_1024TOMAX_OCTETS_E:
                case CPSS_EXCESSIVE_COLLISIONS_E:
                case CPSS_BRDC_PKTS_SENT_E:
                case CPSS_FC_SENT_E:
                case CPSS_GOOD_FC_RCV_E:
                case CPSS_DROP_EVENTS_E:
                case CPSS_UNDERSIZE_PKTS_E:
                case CPSS_FRAGMENTS_PKTS_E:
                case CPSS_OVERSIZE_PKTS_E:
                case CPSS_JABBER_PKTS_E:
                case CPSS_MAC_RCV_ERROR_E:
                case CPSS_BAD_CRC_E:
                case CPSS_COLLISIONS_E:
                case CPSS_LATE_COLLISIONS_E:
                case CPSS_BadFC_RCV_E:
                case CPSS_GOOD_UC_PKTS_RCV_E:
                case CPSS_GOOD_UC_PKTS_SENT_E:
                case CPSS_MULTIPLE_PKTS_SENT_E:
                case CPSS_DEFERRED_PKTS_SENT_E:
                    if(xlgMibShadowPtr->mibShadow[gtMacCounterOffset[cntrName]/4] != (GT_U32)0)
                    {
                        tobeReset = GT_FALSE;
                    }
                    break;
                default:
                    break;
            }
            if(GT_FALSE == tobeReset)
            {
                break;
            }
    }
    if(GT_TRUE == tobeReset)
    {
        for (cntrName = 0 ; cntrName < CPSS_LAST_MAC_COUNTER_NUM_E ; cntrName++)
        {
            xlgMibShadowPtr->mibShadow[cntrName] = (GT_U32)0;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortMacCountersOnPortGet
*
* DESCRIPTION:
*       Gets Ethernet MAC counter for a particular Port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number or CPU port
*
* OUTPUTS:
*       portMacCounterSetArrayPtr - (pointer to) array of current counter values.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     1. The following counters are not supported:
*        Tri-Speed Ports:
*          badPktsRcv, unrecogMacCntrRcv, badFcRcv, goodPktsRcv, goodPktsSent.
*        XG / HyperG.Stack / XLG Potrs:
*          badPktsRcv, unrecogMacCntrRcv, badFcRcv, goodPktsRcv, goodPktsSent,
*          excessiveCollisions, collisions, lateCollisions, deferredPktsSent.
*     2. CPU port counters are valid only when using "Ethernet CPU port", i.e.
*          not using SDMA interface.
*        When using SDMA interface the following APIs are relevant:
*          cpssDxChNetIfSdmaRxCountersGet, cpssDxChNetIfSdmaRxErrorCountGet.
*     3. The following counters are supported for CPU port:
*           goodPktsSent, macTransmitErr, goodOctetsSent, dropEvents,
*           goodPktsRcv, badPktsRcv, goodOctetsRcv, badOctetsRcv.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortMacCountersOnPortGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT CPSS_PORT_MAC_COUNTER_SET_STC   *portMacCounterSetArrayPtr
)
{
    return prvCpssDxChPortMacCountersOnPortGet(devNum,portNum, GT_FALSE,
                                               portMacCounterSetArrayPtr,
                                               PRV_DXCH_PORT_MAC_CNTR_READ_MODE_USUAL_E);
}

/*******************************************************************************
* cpssDxChPortMacCountersOnPortGet
*
* DESCRIPTION:
*       Gets Ethernet MAC counter for a particular Port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number or CPU port
*
* OUTPUTS:
*       portMacCounterSetArrayPtr - (pointer to) array of current counter values.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     1. The following counters are not supported:
*        Tri-Speed Ports:
*          badPktsRcv, unrecogMacCntrRcv, badFcRcv, goodPktsRcv, goodPktsSent.
*        XG / HyperG.Stack / XLG Potrs:
*          badPktsRcv, unrecogMacCntrRcv, badFcRcv, goodPktsRcv, goodPktsSent,
*          excessiveCollisions, collisions, lateCollisions, deferredPktsSent.
*     2. CPU port counters are valid only when using "Ethernet CPU port", i.e.
*          not using SDMA interface.
*        When using SDMA interface the following APIs are relevant:
*          cpssDxChNetIfSdmaRxCountersGet, cpssDxChNetIfSdmaRxErrorCountGet.
*     3. The following counters are supported for CPU port:
*           goodPktsSent, macTransmitErr, goodOctetsSent, dropEvents,
*           goodPktsRcv, badPktsRcv, goodOctetsRcv, badOctetsRcv.
*
*******************************************************************************/
GT_STATUS   cpssDxChPortMacCountersOnPortGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT CPSS_PORT_MAC_COUNTER_SET_STC   *portMacCounterSetArrayPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortMacCountersOnPortGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, portMacCounterSetArrayPtr));

    rc = internal_cpssDxChPortMacCountersOnPortGet(devNum, portNum, portMacCounterSetArrayPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, portMacCounterSetArrayPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortMacCounterCaptureGet
*
* DESCRIPTION:
*       Gets the captured Ethernet MAC counter for a specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       cntrName - specific counter name
*
* OUTPUTS:
*       cntrValuePtr - (pointer to) current counter value.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_BAD_PARAM - on wrong port number, device or counter name
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     1. For Tri-speed ports, each MAC counter capture performed on ports of
*        the same group overrides previous capture made on port of that group.
*         Ports 0-5 belong to group 0.
*         Ports 6-11 belong to group 1.
*         Ports 12-17 belong to group 2.
*         Ports 18-23 belong to group 3.
*     2. The following counters are not supported:
*        Tri-Speed Ports:
*          CPSS_BAD_PKTS_RCV_E, CPSS_UNRECOG_MAC_CNTR_RCV_E, CPSS_BadFC_RCV_E,
*          CPSS_GOOD_PKTS_RCV_E and CPSS_GOOD_PKTS_SENT_E.
*        XG / HyperG.Stack / XLG Potrs:
*          CPSS_BAD_PKTS_RCV_E, CPSS_UNRECOG_MAC_CNTR_RCV_E, CPSS_BadFC_RCV_E,
*          CPSS_GOOD_PKTS_RCV_E, CPSS_GOOD_PKTS_SENT_E,
*          CPSS_EXCESSIVE_COLLISIONS_E, CPSS_COLLISIONS_E,
*          CPSS_LATE_COLLISIONS_E and CPSS_DEFERRED_PKTS_SENT_E.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortMacCounterCaptureGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  CPSS_PORT_MAC_COUNTERS_ENT  cntrName,
    OUT GT_U64                      *cntrValuePtr
)
{
    if((GT_U32)cntrName >= (GT_U32)CPSS_LAST_MAC_COUNTER_NUM_E)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    return prvCpssDxChMacCounterGet(devNum, portNum, cntrName,
                                    GT_TRUE, cntrValuePtr);
}

/*******************************************************************************
* cpssDxChPortMacCounterCaptureGet
*
* DESCRIPTION:
*       Gets the captured Ethernet MAC counter for a specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       cntrName - specific counter name
*
* OUTPUTS:
*       cntrValuePtr - (pointer to) current counter value.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_BAD_PARAM - on wrong port number, device or counter name
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     1. For Tri-speed ports, each MAC counter capture performed on ports of
*        the same group overrides previous capture made on port of that group.
*         Ports 0-5 belong to group 0.
*         Ports 6-11 belong to group 1.
*         Ports 12-17 belong to group 2.
*         Ports 18-23 belong to group 3.
*     2. The following counters are not supported:
*        Tri-Speed Ports:
*          CPSS_BAD_PKTS_RCV_E, CPSS_UNRECOG_MAC_CNTR_RCV_E, CPSS_BadFC_RCV_E,
*          CPSS_GOOD_PKTS_RCV_E and CPSS_GOOD_PKTS_SENT_E.
*        XG / HyperG.Stack / XLG Potrs:
*          CPSS_BAD_PKTS_RCV_E, CPSS_UNRECOG_MAC_CNTR_RCV_E, CPSS_BadFC_RCV_E,
*          CPSS_GOOD_PKTS_RCV_E, CPSS_GOOD_PKTS_SENT_E,
*          CPSS_EXCESSIVE_COLLISIONS_E, CPSS_COLLISIONS_E,
*          CPSS_LATE_COLLISIONS_E and CPSS_DEFERRED_PKTS_SENT_E.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacCounterCaptureGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  CPSS_PORT_MAC_COUNTERS_ENT  cntrName,
    OUT GT_U64                      *cntrValuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortMacCounterCaptureGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, cntrName, cntrValuePtr));

    rc = internal_cpssDxChPortMacCounterCaptureGet(devNum, portNum, cntrName, cntrValuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, cntrName, cntrValuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortMacCountersCaptureOnPortGet
*
* DESCRIPTION:
*       Gets captured Ethernet MAC counter for a particular Port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
* OUTPUTS:
*       portMacCounterSetArrayPtr - (pointer to) array of current counter values.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     1. For Tri-speed ports, each MAC counter capture performed on ports of
*        the same group overrides previous capture made on port of that group.
*         Ports 0-5 belong to group 0.
*         Ports 6-11 belong to group 1.
*         Ports 12-17 belong to group 2.
*         Ports 18-23 belong to group 3.
*     2. The following counters are not supported:
*        Tri-Speed Ports:
*          badPktsRcv, unrecogMacCntrRcv, badFcRcv, goodPktsRcv, goodPktsSent.
*        XG / HyperG.Stack / XLG Potrs:
*          badPktsRcv, unrecogMacCntrRcv, badFcRcv, goodPktsRcv, goodPktsSent,
*          excessiveCollisions, collisions, lateCollisions, deferredPktsSent.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortMacCountersCaptureOnPortGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT CPSS_PORT_MAC_COUNTER_SET_STC   *portMacCounterSetArrayPtr
)
{
    return prvCpssDxChPortMacCountersOnPortGet(devNum,portNum, GT_TRUE,
                                               portMacCounterSetArrayPtr,
                                               PRV_DXCH_PORT_MAC_CNTR_READ_MODE_USUAL_E);
}

/*******************************************************************************
* cpssDxChPortMacCountersCaptureOnPortGet
*
* DESCRIPTION:
*       Gets captured Ethernet MAC counter for a particular Port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
* OUTPUTS:
*       portMacCounterSetArrayPtr - (pointer to) array of current counter values.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     1. For Tri-speed ports, each MAC counter capture performed on ports of
*        the same group overrides previous capture made on port of that group.
*         Ports 0-5 belong to group 0.
*         Ports 6-11 belong to group 1.
*         Ports 12-17 belong to group 2.
*         Ports 18-23 belong to group 3.
*     2. The following counters are not supported:
*        Tri-Speed Ports:
*          badPktsRcv, unrecogMacCntrRcv, badFcRcv, goodPktsRcv, goodPktsSent.
*        XG / HyperG.Stack / XLG Potrs:
*          badPktsRcv, unrecogMacCntrRcv, badFcRcv, goodPktsRcv, goodPktsSent,
*          excessiveCollisions, collisions, lateCollisions, deferredPktsSent.
*
*******************************************************************************/
GT_STATUS   cpssDxChPortMacCountersCaptureOnPortGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT CPSS_PORT_MAC_COUNTER_SET_STC   *portMacCounterSetArrayPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortMacCountersCaptureOnPortGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, portMacCounterSetArrayPtr));

    rc = internal_cpssDxChPortMacCountersCaptureOnPortGet(devNum, portNum, portMacCounterSetArrayPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, portMacCounterSetArrayPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortMacCountersCaptureTriggerSet
*
* DESCRIPTION:
*     The function triggers a capture of MIB counters for specific port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number, whose counters are to be captured.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_STATE - previous capture operation isn't finished.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*     1. For Tri-speed ports, each MAC counter capture performed on ports of
*        the same group overrides previous capture made on port of that group.
*         Ports 0-5 belong to group 0.
*         Ports 6-11 belong to group 1.
*         Ports 12-17 belong to group 2.
*         Ports 18-23 belong to group 3.
*     2. To get the counters, see
*          cpssDxChPortMacCountersCaptureOnPortGet,
*          cpssDxChPortMacCounterCaptureGet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortMacCountersCaptureTriggerSet
(
    IN  GT_U8  devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum
)
{
    GT_U32      regAddr; /* register address to set */
    GT_U32      value;   /* register field value */
    GT_U32      group;   /* Port's group (GOP) */
    GT_U32      offset;  /* bit's offset */
    GT_STATUS   rc;      /* return status */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32 useAsGePort;/* does this port uses the GE port mechanism or the XG mechanism */
    GT_U32 xlgMibEntry[PRV_CPSS_XLG_MIB_COUNTERS_ENTRY_SIZE_CNS];
    PRV_CPSS_DXCH_PORT_XLG_MIB_SHADOW_STC *xlgMibShadowPtr;
    GT_U32          portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    if (PRV_CPSS_DXCH_XCAT2_STACK_MAC_COUNT_NO_CLEAR_ON_READ_WA_MAC(devNum, portNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);

    /* XLG/MSM MIB counters don't support single counter read, clear on read configuration */
    /* and capture. We emulate this features using shadow MIB entries. */
    if(IS_MIB_API_SW_EMULATED_MAC(devNum,portMacNum))
    {
        xlgMibShadowPtr = PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMibShadowArr[portMacNum];
        /* read the whole entry */
        rc = prvCpssDrvHwPpPortGroupReadRam(devNum,portGroupId,
              PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portMacNum].macCounters,
              PRV_CPSS_XLG_MIB_COUNTERS_ENTRY_SIZE_CNS,xlgMibEntry);
        if (rc != GT_OK)
        {
            return rc;
        }

        prvCpssDxChPortSumMibEntry(xlgMibShadowPtr->mibShadow,xlgMibEntry);
        cpssOsMemCpy(xlgMibShadowPtr->captureMibShadow,xlgMibShadowPtr->mibShadow,sizeof(xlgMibShadowPtr->captureMibShadow));

        return GT_OK;
    }
    useAsGePort = IS_GE_PORT_COUNTERS_MAC(devNum,portMacNum);

    /* flex ports uses counters like the XG ports */
    if (useAsGePort)
    {
        group = (PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portMacNum) / 6);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.macCountersCtrl[group] ;


        offset = 0;


    }
    else
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
        {
            switch(portNum){
                case 24:
                    offset = 3;
                    break;
                case 25:
                    offset = 4;
                    break;
                case 26:
                    offset = 5;
                    break;
                case 27:
                    offset = 20;
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }

           regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.hgXsmiiCountersCtr;

        }
        /* Cheetah3 and above */
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                macRegs.perPortRegs[portMacNum].xgMibCountersCtrl;
            offset = 0;
        }
    }

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId,
            regAddr,offset,1,&value);

    if(rc != GT_OK)
    {
        return rc;
    }
    /* Previous capture operation isn't finished.*/
    if(value != 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }


    if (useAsGePort)
    {
        /* Set capture port. */
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,
                regAddr,1,3,(portNum % 6));

        if(rc != GT_OK)
        {
            return rc;
        }

    }

    /* Set cature trigger. */
    return prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,
            regAddr,offset,1,1);

}

/*******************************************************************************
* cpssDxChPortMacCountersCaptureTriggerSet
*
* DESCRIPTION:
*     The function triggers a capture of MIB counters for specific port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number, whose counters are to be captured.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_STATE - previous capture operation isn't finished.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*     1. For Tri-speed ports, each MAC counter capture performed on ports of
*        the same group overrides previous capture made on port of that group.
*         Ports 0-5 belong to group 0.
*         Ports 6-11 belong to group 1.
*         Ports 12-17 belong to group 2.
*         Ports 18-23 belong to group 3.
*     2. To get the counters, see
*          cpssDxChPortMacCountersCaptureOnPortGet,
*          cpssDxChPortMacCounterCaptureGet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacCountersCaptureTriggerSet
(
    IN  GT_U8  devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortMacCountersCaptureTriggerSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum));

    rc = internal_cpssDxChPortMacCountersCaptureTriggerSet(devNum, portNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortMacCountersCaptureTriggerGet
*
* DESCRIPTION:
*    The function gets status of a capture of MIB counters for specific port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number.
*
* OUTPUTS:
*       captureIsDonePtr     - pointer to status of Capture counter Trigger
*                              - GT_TRUE  - capture is done.
*                              - GT_FALSE - capture action is in proccess.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong device number
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     1. For Tri-speed ports, each MAC counter capture performed on ports of
*        the same group overrides previous capture made on port of that group.
*         Ports 0-5 belong to group 0.
*         Ports 6-11 belong to group 1.
*         Ports 12-17 belong to group 2.
*         Ports 18-23 belong to group 3.
*     2. To get the counters, see
*          cpssDxChPortMacCountersCaptureOnPortGet,
*          cpssDxChPortMacCounterCaptureGet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortMacCountersCaptureTriggerGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    OUT GT_BOOL  *captureIsDonePtr
)
{
    GT_U32      regAddr; /* register address to set */
    GT_U32      group;   /* Port's group (GOP) */
    GT_U32      offset;  /* bit's offset */
    GT_U32      value;   /* register field value */
    GT_STATUS   rc;      /* return status */
    GT_U32 useAsGePort;/* does this port uses the GE port mechanism or the XG mechanism */
    GT_U32          portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(captureIsDonePtr);

    /* XLG/MSM MIB counters don't support single counter read, clear on read configuration */
    /* and capture. We emulate this features using shadow MIB entries. */
    if(IS_MIB_API_SW_EMULATED_MAC(devNum,portMacNum))
    {
        *captureIsDonePtr = GT_TRUE;
        return GT_OK;
    }

    useAsGePort = IS_GE_PORT_COUNTERS_MAC(devNum,portMacNum);

    if(useAsGePort)
    {
        group = (PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portMacNum) / 6);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.macCountersCtrl[group] ;


        offset = 0;


    }
    else
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
        {
            switch(portNum){
                case 24:
                    offset = 3;
                    break;
                case 25:
                    offset = 4;
                    break;
                case 26:
                    offset = 5;
                    break;
                case 27:
                    offset = 20;
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }

           regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.hgXsmiiCountersCtr;

        }
        /* Cheetah3 and above */
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                macRegs.perPortRegs[portMacNum].xgMibCountersCtrl;
            offset = 0;
        }

    }

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum),
            regAddr,offset,1,&value);
    if(rc != GT_OK)
    {
        return rc;
    }

    *captureIsDonePtr = (value == 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortMacCountersCaptureTriggerGet
*
* DESCRIPTION:
*    The function gets status of a capture of MIB counters for specific port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number.
*
* OUTPUTS:
*       captureIsDonePtr     - pointer to status of Capture counter Trigger
*                              - GT_TRUE  - capture is done.
*                              - GT_FALSE - capture action is in proccess.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong device number
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     1. For Tri-speed ports, each MAC counter capture performed on ports of
*        the same group overrides previous capture made on port of that group.
*         Ports 0-5 belong to group 0.
*         Ports 6-11 belong to group 1.
*         Ports 12-17 belong to group 2.
*         Ports 18-23 belong to group 3.
*     2. To get the counters, see
*          cpssDxChPortMacCountersCaptureOnPortGet,
*          cpssDxChPortMacCounterCaptureGet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacCountersCaptureTriggerGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    OUT GT_BOOL  *captureIsDonePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortMacCountersCaptureTriggerGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, captureIsDonePtr));

    rc = internal_cpssDxChPortMacCountersCaptureTriggerGet(devNum, portNum, captureIsDonePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, captureIsDonePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* prvCpssDxChLion3PortEgressCntrModeSet
*
* DESCRIPTION:
*       Configure a set of egress counters that work in specified bridge egress
*       counters mode on specified device.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - physical device number
*       cntrSetNum - counter set number.
*       setModeBmp - counter mode bitmap. For example:
*                   GT_EGRESS_CNT_PORT | GT_EGRESS_CNT_VLAN | GT_EGRESS_CNT_DP.
*       portNum   - port number, if corresponding bit in setModeBmp is 1.
*       vlanId    - VLAN Id, if corresponding bit in setModeBmp is 1.
*       tc        - traffic class queue (APPLICABLE RANGES: 0..7),
*                   if corresponding bit in setModeBmp is 1.
*       dpLevel   - drop precedence level, if corresponding bit in setModeBmp is 1.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported DP levels:  CPSS_DP_GREEN_E, CPSS_DP_RED_E.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChLion3PortEgressCntrModeSet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           cntrSetNum,
    IN  CPSS_PORT_EGRESS_CNT_MODE_ENT   setModeBmp,
    IN  GT_PORT_NUM                     portNum,
    IN  GT_U16                          vlanId,
    IN  GT_U8                           tc,
    IN  CPSS_DP_LEVEL_ENT               dpLevel
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32 regAddr;         /* register address */
    GT_U32 regAddr1;        /* register address */
    GT_U32 value;           /* register value*/
    GT_U32 value1;          /* register value*/
    GT_U32 dxDpLevel;       /* DP level value */

    regAddr = PRV_DXCH_REG1_UNIT_TXQ_Q_MAC(devNum).peripheralAccess.egrMIBCntrs.txQMIBCntrsSetConfig[cntrSetNum];
    regAddr1 = PRV_DXCH_REG1_UNIT_TXQ_Q_MAC(devNum).peripheralAccess.egrMIBCntrs.txQMIBCntrsPortSetConfig[cntrSetNum];

    value = setModeBmp & 0xE;  /* vlan, tc, dp */
    value1 = setModeBmp & 0x1; /* port */

    if (setModeBmp & CPSS_EGRESS_CNT_VLAN_E)
    {
        PRV_CPSS_DXCH_VLAN_VALUE_CHECK_MAC(devNum, vlanId);

        /* write the port monitored by set */
        value |= (vlanId & 0xFFFF) << 10;
    }
    if (setModeBmp & CPSS_EGRESS_CNT_TC_E)
    {
        PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tc);

        /* write the port monitored by set */
        value |= (tc & 0x7) << 26;
    }
    if (setModeBmp & CPSS_EGRESS_CNT_DP_E)
    {
        PRV_CPSS_DXCH_COS_DP_TO_HW_CHECK_AND_CONVERT_MAC(
            devNum, dpLevel, dxDpLevel);

        /* write the port monitored by set */
        value |= dxDpLevel << 29;
    }

    rc = prvCpssHwPpWriteRegister(devNum, regAddr, value);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (setModeBmp & CPSS_EGRESS_CNT_PORT_E)
    {
        /* in e_arch this is ePort and not physical port */
        PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum, portNum);

        value1 |= portNum << 4;
    }
    rc = prvCpssHwPpWriteRegister(devNum, regAddr1, value1);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* save the info for 'get' configuration */
    PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.
        portEgressCntrModeInfo[cntrSetNum].portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

    return GT_OK;
}


/*******************************************************************************
* internal_cpssDxChPortEgressCntrModeSet
*
* DESCRIPTION:
*       Configure a set of egress counters that work in specified bridge egress
*       counters mode on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       cntrSetNum - counter set number.
*       setModeBmp - counter mode bitmap. For example:
*                   GT_EGRESS_CNT_PORT | GT_EGRESS_CNT_VLAN | GT_EGRESS_CNT_DP.
*       portNum   - physical port number, if corresponding bit in setModeBmp is 1.
*       vlanId    - VLAN Id, if corresponding bit in setModeBmp is 1.
*       tc        - traffic class queue (0..7),
*                   if corresponding bit in setModeBmp is 1.
*       dpLevel   - drop precedence level, if corresponding bit in setModeBmp is 1.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported DP levels:  CPSS_DP_GREEN_E, CPSS_DP_RED_E.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortEgressCntrModeSet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           cntrSetNum,
    IN  CPSS_PORT_EGRESS_CNT_MODE_ENT   setModeBmp,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  GT_U16                          vlanId,
    IN  GT_U8                           tc,
    IN  CPSS_DP_LEVEL_ENT               dpLevel
)
{
    GT_STATUS   rc;
    GT_U32 regAddr;         /* register address */
    GT_U32 dxDpLevel;/* DP level value */
    GT_U32 value;/* register value*/
    GT_U32  portPortGroupId; /*the port group Id for 'port mode' */
    GT_U32  portValue;/* tmp register value in 'port mode' */
    GT_U32  i;                /* iterator */
    GT_U32  portHwValue;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* only 2 sets supported for now */
    if(cntrSetNum > 1)
       CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    if(setModeBmp &~ (
                CPSS_EGRESS_CNT_PORT_E |
                CPSS_EGRESS_CNT_VLAN_E |
                CPSS_EGRESS_CNT_TC_E   |
                CPSS_EGRESS_CNT_DP_E
                ))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        return prvCpssDxChLion3PortEgressCntrModeSet(devNum, cntrSetNum, setModeBmp, portNum, vlanId, tc, dpLevel);
    }
    else
    {

        if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        egrTxQConf.txQCountSet[cntrSetNum].txQConfig;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        txqVer1.queue.peripheralAccess.egressMibCounterSet.config[cntrSetNum];
        }

        value = setModeBmp & 0xF;

        if (setModeBmp & CPSS_EGRESS_CNT_VLAN_E)
        {
            PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);

            /* write the port monitored by set */
            value |= (vlanId & 0xFFF) << 10;
        }
        if (setModeBmp & CPSS_EGRESS_CNT_TC_E)
        {
            PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tc);

            /* write the port monitored by set */
            value |= (tc & 0x7) << 22;
        }
        if (setModeBmp & CPSS_EGRESS_CNT_DP_E)
        {
            PRV_CPSS_DXCH_COS_DP_TO_HW_CHECK_AND_CONVERT_MAC(
                devNum, dpLevel, dxDpLevel);

            /* write the port monitored by set */
            value |= dxDpLevel << 25;
        }

        if (setModeBmp & CPSS_EGRESS_CNT_PORT_E)
        {
            PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

            /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
            portPortGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

            if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
            {
                /* write the port monitored by set */
                portValue =  (portNum & 0x3F) << 4;

                rc = prvCpssHwPpPortGroupWriteRegister(devNum, portPortGroupId,regAddr,
                        (value | portValue));

                if(rc != GT_OK)
                {
                    return rc;
                }

            }
            else
            {
                if(portNum == CPSS_CPU_PORT_NUM_CNS)
                {
                    /* CPU port is configured according to SDMA port group
                       Port 15 – port group 0 SDMA  Port 31 – port group 1 SDMA
                       Port 47 – port group 2 SDMA  Port 63 – port group 3 SDMA */
                    portPortGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);
                    portValue = (((portPortGroupId) & 0x3)<< 4) | 0xF;
                }
                else
                {
                    portValue =  PRV_CPSS_DXCH_GLOBAL_TO_HEM_LOCAL_PORT(devNum,portNum);
                }

                /* loop on all TXQ units */
                /* on the TXQ that 'own' the port , set the needed configuration */
                /* on other TXQ put 'NULL port' */
                for (i =0; i < PRV_CPSS_DXCH_PP_HW_INFO_TXQ_UNITS_NUM_MAC(devNum); i++)
                {
                    if (i == portPortGroupId/PRV_CPSS_DXCH_PORT_GROUPS_NUM_IN_HEMISPHERE_CNS)
                    {
                        portHwValue = portValue;
                    }
                    else
                    {
                        portHwValue = PRV_CPSS_DXCH_NULL_PORT_NUM_CNS;
                    }

                    /* write the port monitored by set */
                    portHwValue = portHwValue << 4;

                    rc = prvCpssHwPpPortGroupWriteRegister(devNum,
                           i*PRV_CPSS_DXCH_PORT_GROUPS_NUM_IN_HEMISPHERE_CNS,
                           regAddr, (value | portHwValue));
                    if(rc != GT_OK)
                    {
                        return rc;
                    }
                }
                rc = GT_OK;
            }
        }
        else
        {
            portPortGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

            rc = prvCpssHwPpWriteRegister(devNum, regAddr, value);
        }

        if(rc == GT_OK)
        {
            /* save the info for 'get' configuration */
            PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.
                    portEgressCntrModeInfo[cntrSetNum].portGroupId = portPortGroupId;
        }

        return rc;
    }
}

/*******************************************************************************
* cpssDxChPortEgressCntrModeSet
*
* DESCRIPTION:
*       Configure a set of egress counters that work in specified bridge egress
*       counters mode on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       cntrSetNum - counter set number.
*       setModeBmp - counter mode bitmap. For example:
*                   GT_EGRESS_CNT_PORT | GT_EGRESS_CNT_VLAN | GT_EGRESS_CNT_DP.
*       portNum   - physical port number, if corresponding bit in setModeBmp is 1.
*       vlanId    - VLAN Id, if corresponding bit in setModeBmp is 1.
*       tc        - traffic class queue (0..7),
*                   if corresponding bit in setModeBmp is 1.
*       dpLevel   - drop precedence level, if corresponding bit in setModeBmp is 1.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported DP levels:  CPSS_DP_GREEN_E, CPSS_DP_RED_E.
*
*******************************************************************************/
GT_STATUS cpssDxChPortEgressCntrModeSet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           cntrSetNum,
    IN  CPSS_PORT_EGRESS_CNT_MODE_ENT   setModeBmp,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  GT_U16                          vlanId,
    IN  GT_U8                           tc,
    IN  CPSS_DP_LEVEL_ENT               dpLevel
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortEgressCntrModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cntrSetNum, setModeBmp, portNum, vlanId, tc, dpLevel));

    rc = internal_cpssDxChPortEgressCntrModeSet(devNum, cntrSetNum, setModeBmp, portNum, vlanId, tc, dpLevel);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cntrSetNum, setModeBmp, portNum, vlanId, tc, dpLevel));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* prvCpssDxChLion3PortEgressCntrModeGet
*
* DESCRIPTION:
*       Get configuration of a bridge egress counters set.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - physical device number
*       cntrSetNum - counter set number.
*
* OUTPUTS:
*       setModeBmpPtr - Pointer to counter mode bitmap. For example:
*                       GT_EGRESS_CNT_PORT | GT_EGRESS_CNT_VLAN | GT_EGRESS_CNT_DP.
*       portNumPtr    - Pointer to the egress port of the packets counted by
*                       the set of counters
*       vlanIdPtr     - Pointer to the vlan id of the packets counted by the
*                       set of counters.
*       tcPtr         - Pointer to the tc of the packets counted by the set
*                       of counters.
*       dpLevelPtr    - Pointer to the dp of the packets counted by the set
*                       of counters.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported DP levels:  CPSS_DP_GREEN_E, CPSS_DP_RED_E.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChLion3PortEgressCntrModeGet
(
    IN   GT_U8                           devNum,
    IN   GT_U8                           cntrSetNum,
    OUT  CPSS_PORT_EGRESS_CNT_MODE_ENT   *setModeBmpPtr,
    OUT  GT_PORT_NUM                     *portNumPtr,
    OUT  GT_U16                          *vlanIdPtr,
    OUT  GT_U8                           *tcPtr,
    OUT  CPSS_DP_LEVEL_ENT               *dpLevelPtr
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32 regAddr;         /* register address */
    GT_U32 regAddr1;        /* register address */
    GT_U32 value;           /* register value*/
    GT_U32 value1;          /* register value*/
    GT_U32 dxDpLevel;       /* DP HW value */

    regAddr = PRV_DXCH_REG1_UNIT_TXQ_Q_MAC(devNum).peripheralAccess.egrMIBCntrs.txQMIBCntrsSetConfig[cntrSetNum];
    regAddr1 = PRV_DXCH_REG1_UNIT_TXQ_Q_MAC(devNum).peripheralAccess.egrMIBCntrs.txQMIBCntrsPortSetConfig[cntrSetNum];

    rc = prvCpssHwPpReadRegister(devNum, regAddr, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssHwPpReadRegister(devNum, regAddr1, &value1);
    if (rc != GT_OK)
    {
        return rc;
    }

    *setModeBmpPtr = value1 & 0x1;
    *setModeBmpPtr |= value & 0xE;

    *portNumPtr = U32_GET_FIELD_MAC(value1, 4, 17);
    *vlanIdPtr = U32_GET_FIELD_MAC(value, 10, 16);
    *tcPtr = U32_GET_FIELD_MAC(value, 26, 3);

    dxDpLevel = (value >> 29) & 0x3;

    PRV_CPSS_DXCH_COS_DP_TO_SW_CHECK_AND_CONVERT_MAC(
        devNum, dxDpLevel, (*dpLevelPtr));

    return GT_OK;
}


/*******************************************************************************
* internal_cpssDxChPortEgressCntrModeGet
*
* DESCRIPTION:
*       Get configuration of a bridge egress counters set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       cntrSetNum - counter set number.
*
* OUTPUTS:
*       setModeBmpPtr - Pointer to counter mode bitmap. For example:
*                       GT_EGRESS_CNT_PORT | GT_EGRESS_CNT_VLAN | GT_EGRESS_CNT_DP.
*       portNumPtr    - Pointer to the egress port of the packets counted by
*                       the set of counters
*       vlanIdPtr     - Pointer to the vlan id of the packets counted by the
*                       set of counters.
*       tcPtr         - Pointer to the tc of the packets counted by the set
*                       of counters.
*       dpLevelPtr    - Pointer to the dp of the packets counted by the set
*                       of counters.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported DP levels:  CPSS_DP_GREEN_E, CPSS_DP_RED_E.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortEgressCntrModeGet
(
    IN   GT_U8                           devNum,
    IN   GT_U8                           cntrSetNum,
    OUT  CPSS_PORT_EGRESS_CNT_MODE_ENT   *setModeBmpPtr,
    OUT  GT_PHYSICAL_PORT_NUM            *portNumPtr,
    OUT  GT_U16                          *vlanIdPtr,
    OUT  GT_U8                           *tcPtr,
    OUT  CPSS_DP_LEVEL_ENT               *dpLevelPtr
)
{
    GT_STATUS rc;              /* return code                   */
    GT_U32    regAddr;         /* register address              */
    GT_U32    dxDpLevel;       /* DP HW value                   */
    GT_U32    dxDpLevelMask;   /* DP HW value mask              */
    GT_U32    value;           /* HW value of register contents */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(setModeBmpPtr);
    CPSS_NULL_PTR_CHECK_MAC(portNumPtr);
    CPSS_NULL_PTR_CHECK_MAC(vlanIdPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcPtr);
    CPSS_NULL_PTR_CHECK_MAC(dpLevelPtr);

    /* only 2 sets supported for now */
    if(cntrSetNum > 1)
       CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);


    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        return prvCpssDxChLion3PortEgressCntrModeGet(devNum, cntrSetNum, setModeBmpPtr, portNumPtr, vlanIdPtr, tcPtr, dpLevelPtr);
    }
    else
    {

        /* get the info from DB */
        portGroupId = PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.
                    portEgressCntrModeInfo[cntrSetNum].portGroupId;

        if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        egrTxQConf.txQCountSet[cntrSetNum].txQConfig;
            dxDpLevelMask = 1;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        txqVer1.queue.peripheralAccess.egressMibCounterSet.config[cntrSetNum];
            dxDpLevelMask = 3;
        }

        rc = prvCpssHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &value);

        *setModeBmpPtr = value & 0xF;

        *portNumPtr = (GT_U8)((value >> 4) & 0x3F);
        *vlanIdPtr = (GT_U16)((value >> 10) & 0xFFF);
        *tcPtr = (GT_U8)((value >> 22) & 0x7);


        dxDpLevel = (value >> 25) & dxDpLevelMask;

        PRV_CPSS_DXCH_COS_DP_TO_SW_CHECK_AND_CONVERT_MAC(
            devNum, dxDpLevel, (*dpLevelPtr));

        if(1 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
        {
            /* support multi TXQs device, convert local port to global port */
                *portNumPtr = (GT_U8)( PRV_CPSS_DXCH_HEM_LOCAL_TO_GLOBAL_PORT(devNum,portGroupId,(*portNumPtr)));
        }

        return rc;
    }

}

/*******************************************************************************
* cpssDxChPortEgressCntrModeGet
*
* DESCRIPTION:
*       Get configuration of a bridge egress counters set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       cntrSetNum - counter set number.
*
* OUTPUTS:
*       setModeBmpPtr - Pointer to counter mode bitmap. For example:
*                       GT_EGRESS_CNT_PORT | GT_EGRESS_CNT_VLAN | GT_EGRESS_CNT_DP.
*       portNumPtr    - Pointer to the egress port of the packets counted by
*                       the set of counters
*       vlanIdPtr     - Pointer to the vlan id of the packets counted by the
*                       set of counters.
*       tcPtr         - Pointer to the tc of the packets counted by the set
*                       of counters.
*       dpLevelPtr    - Pointer to the dp of the packets counted by the set
*                       of counters.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported DP levels:  CPSS_DP_GREEN_E, CPSS_DP_RED_E.
*
*******************************************************************************/
GT_STATUS cpssDxChPortEgressCntrModeGet
(
    IN   GT_U8                           devNum,
    IN   GT_U8                           cntrSetNum,
    OUT  CPSS_PORT_EGRESS_CNT_MODE_ENT   *setModeBmpPtr,
    OUT  GT_PHYSICAL_PORT_NUM            *portNumPtr,
    OUT  GT_U16                          *vlanIdPtr,
    OUT  GT_U8                           *tcPtr,
    OUT  CPSS_DP_LEVEL_ENT               *dpLevelPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortEgressCntrModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cntrSetNum, setModeBmpPtr, portNumPtr, vlanIdPtr, tcPtr, dpLevelPtr));

    rc = internal_cpssDxChPortEgressCntrModeGet(devNum, cntrSetNum, setModeBmpPtr, portNumPtr, vlanIdPtr, tcPtr, dpLevelPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cntrSetNum, setModeBmpPtr, portNumPtr, vlanIdPtr, tcPtr, dpLevelPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortEgressCntrsGet
*
* DESCRIPTION:
*       Gets a egress counters from specific counter-set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - physical device number
*       cntrSetNum - counter set number : 0, 1
*
* OUTPUTS:
*       egrCntrPtr - (pointer to) structure of egress counters current values.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortEgressCntrsGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       cntrSetNum,
    OUT CPSS_PORT_EGRESS_CNTR_STC   *egrCntrPtr
)
{
    GT_U32 regAddr;         /* register address */
    CPSS_PORT_EGRESS_CNTR_STC   *egressCntrShadowPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(egrCntrPtr);

    if (cntrSetNum > 1)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    PRV_CPSS_DXCH_LION_BOOKMARK /* need to support PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.peripheralAccess.egressMibCounterSet.multicastFifoDroppedPacketsCounter[cntrSetNum]

                    like in EXMXPM : CPSS_EXMXPM_BRG_EGRESS_CNTR_STC
                    */

    /* read Outgoing Unicast Packet Count Register */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_Q_MAC(devNum).peripheralAccess.egrMIBCntrs.setOutgoingUcPktCntr[cntrSetNum];
    }
    else if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    egrTxQConf.txQCountSet[cntrSetNum].outUcPkts;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.peripheralAccess.egressMibCounterSet.outgoingUnicastPacketCounter[cntrSetNum];
    }

    if (prvCpssPortGroupsCounterSummary(devNum, regAddr,0,32, &egrCntrPtr->outUcFrames,NULL) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    /* read Outgoing Multicast Packet Count Register */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_Q_MAC(devNum).peripheralAccess.egrMIBCntrs.setOutgoingMcPktCntr[cntrSetNum];
    }
    else if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        egrTxQConf.txQCountSet[cntrSetNum].outMcPkts;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.peripheralAccess.egressMibCounterSet.outgoingMulticastPacketCounter[cntrSetNum];
    }


    if(prvCpssPortGroupsCounterSummary(devNum, regAddr,0,32, &egrCntrPtr->outMcFrames,NULL)
       != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    /* read Outgoing Byte Count Packet Count Register */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_Q_MAC(devNum).peripheralAccess.egrMIBCntrs.setOutgoingBcPktCntr[cntrSetNum];
    }
    else if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        egrTxQConf.txQCountSet[cntrSetNum].outBytePkts;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.peripheralAccess.egressMibCounterSet.outgoingBroadcastPacketCounter[cntrSetNum];
    }

    if (prvCpssPortGroupsCounterSummary(devNum, regAddr,0,32, &egrCntrPtr->outBcFrames,NULL) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    /* read Bridge Egress Filtered Packet Count Register */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_Q_MAC(devNum).peripheralAccess.egrMIBCntrs.setBridgeEgrFilteredPktCntr[cntrSetNum];
    }
    else if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        egrTxQConf.txQCountSet[cntrSetNum].brgEgrFiltered;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.peripheralAccess.egressMibCounterSet.bridgeEgressFilteredPacketCounter[cntrSetNum];
    }

    if (prvCpssPortGroupsCounterSummary(devNum, regAddr,0,32, &egrCntrPtr->brgEgrFilterDisc,NULL) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    /* read Transmit Queue Filtered Packet Count Register */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_Q_MAC(devNum).peripheralAccess.egrMIBCntrs.setTailDroppedPktCntr[cntrSetNum];
    }
    else if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        egrTxQConf.txQCountSet[cntrSetNum].txQFiltered;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.peripheralAccess.egressMibCounterSet.tailDroppedPacketCounter[cntrSetNum];
    }

    if (prvCpssPortGroupsCounterSummary(devNum, regAddr,0,32, &egrCntrPtr->txqFilterDisc,NULL) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    /* read  to cpu, from cpu and to analyzer Packets Count Register*/
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_Q_MAC(devNum).peripheralAccess.egrMIBCntrs.setCtrlPktCntr[cntrSetNum];
    }
    else if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        egrTxQConf.txQCountSet[cntrSetNum].outCtrlPkts;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.peripheralAccess.egressMibCounterSet.controlPacketCounter[cntrSetNum];
    }

    if (prvCpssPortGroupsCounterSummary(devNum, regAddr,0,32, &egrCntrPtr->outCtrlFrames,NULL) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    if((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E) && (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) != GT_TRUE))
    {
        /* read  egress forwarding restriction dropped Packets Count Register
           Supported from ch2 devices.*/
        if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            egrTxQConf.txQCountSet[cntrSetNum].egrFrwDropPkts;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        txqVer1.queue.peripheralAccess.egressMibCounterSet.
                            egressForwardingRestrictionDroppedPacketsCounter[cntrSetNum];
        }

        if (prvCpssPortGroupsCounterSummary(devNum, regAddr,0,32, &egrCntrPtr->egrFrwDropFrames,NULL) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        egrCntrPtr->egrFrwDropFrames = 0;
    }

    /* read Transmit Queue Filtered Packet Count Register */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_Q_MAC(devNum).peripheralAccess.egrMIBCntrs.setMcFIFO3_0DroppedPktsCntr[cntrSetNum];

        if (prvCpssPortGroupsCounterSummary(devNum, regAddr,0,32, &egrCntrPtr->mcFifo3_0DropPkts,NULL) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

        regAddr = PRV_DXCH_REG1_UNIT_TXQ_Q_MAC(devNum).peripheralAccess.egrMIBCntrs.setMcFIFO7_4DroppedPktsCntr[cntrSetNum];

        if (prvCpssPortGroupsCounterSummary(devNum, regAddr,0,32, &egrCntrPtr->mcFifo7_4DropPkts,NULL) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

    }
    else
    {
        egrCntrPtr->mcFifo3_0DropPkts = 0;
        egrCntrPtr->mcFifo7_4DropPkts = 0;
    }



    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(
                        devNum,
                        PRV_CPSS_DXCH_BOBCAT2_EGRESS_MIB_COUNTERS_NOT_ROC_WA_E))
     {
        egressCntrShadowPtr = &(PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                info_PRV_CPSS_DXCH_BOBCAT2_EGRESS_MIB_COUNTERS_NOT_ROC_WA_E.
                      egressCntrShadow[cntrSetNum]);

        egrCntrPtr->outUcFrames      -= egressCntrShadowPtr->outUcFrames;
        egrCntrPtr->outMcFrames      -= egressCntrShadowPtr->outMcFrames;
        egrCntrPtr->outBcFrames      -= egressCntrShadowPtr->outBcFrames;
        egrCntrPtr->brgEgrFilterDisc -= egressCntrShadowPtr->brgEgrFilterDisc;
        egrCntrPtr->txqFilterDisc    -= egressCntrShadowPtr->txqFilterDisc;
        egrCntrPtr->outCtrlFrames    -= egressCntrShadowPtr->outCtrlFrames;
        egrCntrPtr->egrFrwDropFrames -= egressCntrShadowPtr->egrFrwDropFrames;
        egrCntrPtr->mcFifo3_0DropPkts -= egressCntrShadowPtr->mcFifo3_0DropPkts;
        egrCntrPtr->mcFifo7_4DropPkts -= egressCntrShadowPtr->mcFifo7_4DropPkts;

        egressCntrShadowPtr->outUcFrames      += egrCntrPtr->outUcFrames;
        egressCntrShadowPtr->outMcFrames      += egrCntrPtr->outMcFrames;
        egressCntrShadowPtr->outBcFrames      += egrCntrPtr->outBcFrames;
        egressCntrShadowPtr->brgEgrFilterDisc += egrCntrPtr->brgEgrFilterDisc;
        egressCntrShadowPtr->txqFilterDisc    += egrCntrPtr->txqFilterDisc;
        egressCntrShadowPtr->outCtrlFrames    += egrCntrPtr->outCtrlFrames;
        egressCntrShadowPtr->egrFrwDropFrames += egrCntrPtr->egrFrwDropFrames;
        egressCntrShadowPtr->mcFifo3_0DropPkts += egrCntrPtr->mcFifo3_0DropPkts;
        egressCntrShadowPtr->mcFifo7_4DropPkts += egrCntrPtr->mcFifo7_4DropPkts;

     }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortEgressCntrsGet
*
* DESCRIPTION:
*       Gets a egress counters from specific counter-set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - physical device number
*       cntrSetNum - counter set number : 0, 1
*
* OUTPUTS:
*       egrCntrPtr - (pointer to) structure of egress counters current values.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortEgressCntrsGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       cntrSetNum,
    OUT CPSS_PORT_EGRESS_CNTR_STC   *egrCntrPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortEgressCntrsGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cntrSetNum, egrCntrPtr));

    rc = internal_cpssDxChPortEgressCntrsGet(devNum, cntrSetNum, egrCntrPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cntrSetNum, egrCntrPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortMacCountersEnable
*
* DESCRIPTION:
*       Enable or disable MAC Counters update for this port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number.
*       portNum  - physical port number (or CPU port)
*       enable   -  enable update of MAC counters
*                   GT_FALSE = MAC counters update for this port is disabled.
*                   GT_TRUE = MAC counters update for this port is enabled.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None;
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortMacCountersEnable
(
    IN GT_U8 devNum,
    IN GT_PHYSICAL_PORT_NUM        portNum,
    IN GT_BOOL enable
)
{
    GT_U32  regAddr;/* register address to set */
    GT_U32  value;/* value to set to bits */
    GT_U32  offset;/* field offset */
    GT_U32     portMacNum; /* MAC number */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    offset = 15;
    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_GE_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_GE_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldData = BOOL2BIT_MAC(enable);
        regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = 15;
    }

    value = BOOL2BIT_MAC(!enable);
    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        offset = 14;
    }
    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldOffset = offset;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XLG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XLG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldOffset = offset;
    }

    return prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
}

/*******************************************************************************
* cpssDxChPortMacCountersEnable
*
* DESCRIPTION:
*       Enable or disable MAC Counters update for this port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number.
*       portNum  - physical port number (or CPU port)
*       enable   -  enable update of MAC counters
*                   GT_FALSE = MAC counters update for this port is disabled.
*                   GT_TRUE = MAC counters update for this port is enabled.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None;
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacCountersEnable(
    IN GT_U8 devNum,
    IN GT_PHYSICAL_PORT_NUM        portNum,
    IN GT_BOOL enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortMacCountersEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChPortMacCountersEnable(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortMacCountersClearOnReadSet
*
* DESCRIPTION:
*       Enable or disable MAC Counters Clear on read per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*       - Ports 0 through 5
*       - Ports 6 through 11
*       - Ports 12 through 17
*       - Ports 18 through 23
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - physical port number (or CPU port)
*       enable      - enable clear on read for MAC counters
*                   GT_FALSE - Counters are not cleared.
*                   GT_TRUE - Counters are cleared.

*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortMacCountersClearOnReadSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM        portNum,
    IN GT_BOOL  enable
)
{
    GT_U32      regAddr;/* register address to set */
    GT_U32      value;  /* value to set to bits */
    GT_U32      group;  /* Port's group (GOP) */
    GT_U32      offset; /* bit's offset */
    GT_STATUS   rc;     /* return code */
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_DXCH_ERRATA_STACK_MAC_COUNT_SHADOW_STC *macCountShadowPtr; /* Pointer To MAC Counters Shadow */
    GT_U32 portMacNum; /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

   /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);

    if(IS_MIB_API_SW_EMULATED_MAC(devNum,portMacNum))
    {
        PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMibShadowArr[portMacNum]->clearOnReadEnable = enable;
        return GT_OK;
    }

    if (PRV_CPSS_DXCH_XCAT2_STACK_MAC_COUNT_NO_CLEAR_ON_READ_WA_MAC(devNum, portMacNum))
    {
        macCountShadowPtr =
        PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
            info_PRV_CPSS_DXCH_XCAT2_STACK_MAC_COUNT_NO_CLEAR_ON_READ_WA_E.
            stackMacCntPtr;
        macCountShadowPtr->stackPortArr[portMacNum - 24].clearOnRead = enable;
        return GT_OK;
    }

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_FALSE &&
        portMacNum == CPSS_CPU_PORT_NUM_CNS)
    {
        value = ((enable == GT_TRUE) ? 1 : 0);
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.cpuPortCtrlReg;
        offset = 3;
        return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,
                                                  regAddr, offset, 1, value);
    }
    else
    {
        value = ((enable == GT_TRUE) ? 0 : 1);
        offset = 4;
        group = (PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portMacNum) / 6);
        if(group < 4)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.macCountersCtrl[group];
            if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
            {
                rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,
                                                        regAddr, offset, 1, value);
                if (rc!=GT_OK)
                {
                    return rc;
                }
            }
        }

        if(PRV_CPSS_PP_MAC(devNum)->devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
        {
            if(portMacNum < 24)
                return GT_OK;

            switch(portMacNum){
            case 24:
                offset = 0;
                break;
            case 25:
                offset = 1;
                break;
            case 26:
                offset = 2;
                break;
            case 27:
                offset = 19;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.hgXsmiiCountersCtr;
        }
        /* Cheetah3 and above */
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                macRegs.perPortRegs[portMacNum].xgMibCountersCtrl;
            offset = 1;
        }

        if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,
                                                      regAddr, offset, 1, value);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortMacCountersClearOnReadSet
*
* DESCRIPTION:
*       Enable or disable MAC Counters Clear on read per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*       - Ports 0 through 5
*       - Ports 6 through 11
*       - Ports 12 through 17
*       - Ports 18 through 23
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - physical port number (or CPU port)
*       enable      - enable clear on read for MAC counters
*                   GT_FALSE - Counters are not cleared.
*                   GT_TRUE - Counters are cleared.

*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChPortMacCountersClearOnReadSet(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM        portNum,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortMacCountersClearOnReadSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChPortMacCountersClearOnReadSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortMacCountersClearOnReadGet
*
* DESCRIPTION:
*       Get "Clear on read" status of MAC Counters per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*       - Ports 0 through 5
*       - Ports 6 through 11
*       - Ports 12 through 17
*       - Ports 18 through 23
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr   - (pointer to) "Clear on read" status for MAC counters
*                     GT_FALSE - Counters are not cleared.
*                     GT_TRUE - Counters are cleared.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_FAIL                     - on error.
*       GT_HW_ERROR                 - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*       GT_BAD_PTR                  - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortMacCountersClearOnReadGet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM        portNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS   rc;      /* return code */
    GT_U32      regAddr; /* register address to get */
    GT_U32      regData; /* register data */
    GT_U32      group;   /* Port's group (GOP) */
    GT_U32      offset;  /* bit's offset */
    GT_U32 useAsGePort;/* does this port uses the GE port mechanism or the XG mechanism */
    PRV_CPSS_DXCH_ERRATA_STACK_MAC_COUNT_SHADOW_STC *macCountShadowPtr; /* Pointer To MAC Counters Shadow */
    GT_U32 portMacNum; /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* XLG/MSM MIB counters don't support single counter read, clear on read configuration */
    /* and capture. We emulate this features using shadow MIB entries. */
    if(IS_MIB_API_SW_EMULATED_MAC(devNum,portMacNum))
    {
        *enablePtr = PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMibShadowArr[portMacNum]->clearOnReadEnable;
        return GT_OK;
    }

    useAsGePort = IS_GE_PORT_COUNTERS_MAC(devNum,portMacNum);

    if (PRV_CPSS_DXCH_XCAT2_STACK_MAC_COUNT_NO_CLEAR_ON_READ_WA_MAC(devNum, portMacNum))
    {
        macCountShadowPtr =
        PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
            info_PRV_CPSS_DXCH_XCAT2_STACK_MAC_COUNT_NO_CLEAR_ON_READ_WA_E.
            stackMacCntPtr;
        *enablePtr = macCountShadowPtr->stackPortArr[portMacNum - 24].clearOnRead;
        return GT_OK;
    }

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_FALSE &&
        portMacNum == CPSS_CPU_PORT_NUM_CNS)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.cpuPortCtrlReg;
        offset = 3;
    }
    else
    {
        if(useAsGePort)
        {
            group = (PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portMacNum) / 6);

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.macCountersCtrl[group] ;
            offset = 4;
        }
        else
        {
            if(PRV_CPSS_PP_MAC(devNum)->devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
            {
                switch(portMacNum){
                    case 24:
                        offset = 0;
                        break;
                    case 25:
                        offset = 1;
                        break;
                    case 26:
                        offset = 2;
                        break;
                    case 27:
                        offset = 19;
                        break;
                    default:
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                }

                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.hgXsmiiCountersCtr;
            }

            /* Cheetah3 and above */
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    macRegs.perPortRegs[portMacNum].xgMibCountersCtrl;
                offset = 1;
            }

        }

    }

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,
            PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum),
            regAddr, offset, 1, &regData);
    if( GT_OK != rc )
    {
        return rc;
    }

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_FALSE &&
        portMacNum == CPSS_CPU_PORT_NUM_CNS)
    {
        *enablePtr = BIT2BOOL_MAC(regData);
    }
    else
    {
        *enablePtr = (regData == 0 ) ? GT_TRUE : GT_FALSE ;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortMacCountersClearOnReadGet
*
* DESCRIPTION:
*       Get "Clear on read" status of MAC Counters per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*       - Ports 0 through 5
*       - Ports 6 through 11
*       - Ports 12 through 17
*       - Ports 18 through 23
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr   - (pointer to) "Clear on read" status for MAC counters
*                     GT_FALSE - Counters are not cleared.
*                     GT_TRUE - Counters are cleared.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_FAIL                     - on error.
*       GT_HW_ERROR                 - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*       GT_BAD_PTR                  - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChPortMacCountersClearOnReadGet(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM        portNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortMacCountersClearOnReadGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChPortMacCountersClearOnReadGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortMacCountersRxHistogramEnable
*
* DESCRIPTION:
*       Enable/disable updating of the RMON Etherstat histogram
*       counters for received packets per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*           - Ports 0 through 5
*           - Ports 6 through 11
*           - Ports 12 through 17
*           - Ports 18 through 23
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - physical port number
*       enable      - enable updating of the counters for received packets
*                   GT_TRUE - Counters are updated.
*                   GT_FALSE - Counters are not updated.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortMacCountersRxHistogramEnable
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM        portNum,
    IN GT_BOOL  enable
)
{
    GT_U32      regAddr;/* register address to set */
    GT_U32      value;/* value to set to bits */
    GT_U32      group;  /* Port's group (GOP) */
    GT_U32      offset; /* bit's offset */
    GT_STATUS rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32          portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);

    value = ((enable == GT_TRUE) ? 1 : 0);

    if((PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(devNum,portMacNum) == GT_FALSE) &&  (portMacNum < 24) )
    {
        group = (PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portMacNum) / 6);

        if(PRV_CPSS_PP_MAC(devNum)->appDevFamily == CPSS_XCAT3_E)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portMacNum].mibCountersCtrl;
            offset = 2;
            if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
            {
                rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,
                                                        regAddr, offset, 1, value);

                if (rc!=GT_OK)
                {
                        return rc;
                }
            }
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.macCountersCtrl[group] ;
        offset = 5;
        if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,
                                                    regAddr, offset, 1, value);

            if (rc!=GT_OK)
            {
                    return rc;
            }
        }
    }

    if(PRV_CPSS_PP_MAC(devNum)->devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
    {
        if (portNum < 24)
            return GT_OK;

        switch(portNum){
        case 24:
            offset = 6;
            break;
        case 25:
            offset = 7;
            break;
        case 26:
            offset = 8;
            break;
        case 27:
            offset = 21;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.hgXsmiiCountersCtr ;
     }
    /* Cheetah3 and above */
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                macRegs.perPortRegs[portMacNum].xgMibCountersCtrl;
        offset = 2;
    }

    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,
                                                      regAddr, offset, 1, value);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                macRegs.perPortRegs[portMacNum].xlgMibCountersCtrl;
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        return prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,
                                                  regAddr, offset, 1, value);
    }

    return GT_OK;

}

/*******************************************************************************
* cpssDxChPortMacCountersRxHistogramEnable
*
* DESCRIPTION:
*       Enable/disable updating of the RMON Etherstat histogram
*       counters for received packets per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*           - Ports 0 through 5
*           - Ports 6 through 11
*           - Ports 12 through 17
*           - Ports 18 through 23
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - physical port number
*       enable      - enable updating of the counters for received packets
*                   GT_TRUE - Counters are updated.
*                   GT_FALSE - Counters are not updated.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChPortMacCountersRxHistogramEnable(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM        portNum,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortMacCountersRxHistogramEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChPortMacCountersRxHistogramEnable(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortMacCountersTxHistogramEnable
*
* DESCRIPTION:
*       Enable/disable updating of the RMON Etherstat histogram
*       counters for transmitted packets per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*           - Ports 0 through 5
*           - Ports 6 through 11
*           - Ports 12 through 17
*           - Ports 18 through 23
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - physical port number
*       enable      - enable updating of the counters for transmitted packets
*                   GT_TRUE - Counters are updated.
*                   GT_FALSE - Counters are not updated.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortMacCountersTxHistogramEnable
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM        portNum,
    IN GT_BOOL  enable
)
{
    GT_U32      regAddr;/* register address to set */
    GT_U32      value;/* value to set to bits */
    GT_U32      group;  /* Port's group (GOP) */
    GT_U32      offset; /* bit's offset */
    GT_STATUS rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32          portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);

    value = ((enable == GT_TRUE) ? 1 : 0);

    if((PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(devNum,portMacNum) == GT_FALSE) &&  (portMacNum < 24) )
    {
        group = (PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portMacNum) / 6);

        if(PRV_CPSS_PP_MAC(devNum)->appDevFamily == CPSS_XCAT3_E)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portMacNum].mibCountersCtrl;
            offset = 3;
            if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
            {
                rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,
                                                        regAddr, offset, 1, value);

                if (rc!=GT_OK)
                {
                        return rc;
                }
            }
        }

       regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.macCountersCtrl[group] ;
       offset = 6;
       if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,
                                                    regAddr, offset, 1, value);

            if (rc!=GT_OK)
            {
                    return rc;
            }
        }
    }

    if(PRV_CPSS_PP_MAC(devNum)->devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
    {
        if(portNum < 24)
            return GT_OK;

        switch(portNum){
        case 24:
            offset = 9;
            break;
        case 25:
            offset = 10;
            break;
        case 26:
            offset = 11;
            break;
        case 27:
            offset = 22;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.hgXsmiiCountersCtr ;
    }
    /* Cheetah3 and above */
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                macRegs.perPortRegs[portMacNum].xgMibCountersCtrl;
        offset = 3;
    }

    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,
                                                  regAddr, offset, 1, value);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                macRegs.perPortRegs[portMacNum].xlgMibCountersCtrl;
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        return prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,
                                                  regAddr, offset, 1, value);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortMacCountersTxHistogramEnable
*
* DESCRIPTION:
*       Enable/disable updating of the RMON Etherstat histogram
*       counters for transmitted packets per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*           - Ports 0 through 5
*           - Ports 6 through 11
*           - Ports 12 through 17
*           - Ports 18 through 23
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - physical port number
*       enable      - enable updating of the counters for transmitted packets
*                   GT_TRUE - Counters are updated.
*                   GT_FALSE - Counters are not updated.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChPortMacCountersTxHistogramEnable(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM        portNum,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortMacCountersTxHistogramEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChPortMacCountersTxHistogramEnable(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortMacOversizedPacketsCounterModeSet
*
* DESCRIPTION:
*       Sets MIB counters behaviour for oversized packets on given port.
*
* APPLICABLE DEVICES:
*        xCat2, xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number
*       counterMode - oversized packets counter mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number, device or counter mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The oversized packets counter mode determines the behavour of the
*       CPSS_PKTS_1024TOMAX_OCTETS_E and CPSS_DEFERRED_PKTS_SENT_E counters.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortMacOversizedPacketsCounterModeSet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_ENT  counterMode
)
{
    GT_U32 regAddr;    /* register address to set */
    GT_U32 value;      /* value to set to bits */
    GT_U32 group;      /* Port's group (GOP) */
    GT_U32 mask;       /* bit's offset */
    GT_U32 portOffset; /* port's offset in register */
    GT_U32 useAsGePort;/* does this port uses the GE port mechanism or the XG mechanism */
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E| CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    if (PRV_CPSS_DXCH_XCAT2_STACK_MAC_COUNT_NO_CLEAR_ON_READ_WA_MAC(devNum, portNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }

    useAsGePort = IS_GE_PORT_COUNTERS_MAC(devNum,portNum);
    if(PRV_CPSS_PP_MAC(devNum)->appDevFamily == CPSS_XCAT2_E)
    {
        switch(counterMode)
        {
            case CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_NORMAL_E:
                value = 0x0; break;
            case CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_HISTOGRAM_1518_E:
                value = 0x1; break;
            case CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_HISTOGRAM_1522_E:
                value = useAsGePort ? 0x41 : 0x3; break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if(useAsGePort)
        {
            group = (PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum) / 6);
            if(group >= 4)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            portOffset = (PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum) % 6);
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.macCountersCtrl[group] ;

            mask = 0x410000 << portOffset;
            value <<=  16 + portOffset;

            rc = prvCpssDrvHwPpPortGroupWriteRegBitMask(devNum,
              PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
              regAddr,mask,value);
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
              macRegs.perPortRegs[portNum].xgMibCountersCtrl;
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,
              PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
              regAddr,6,2,value);
        }
    }
    else if(PRV_CPSS_PP_MAC(devNum)->appDevFamily == CPSS_XCAT3_E)
    {
        switch(counterMode)
        {
            case CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_NORMAL_E:
                value = 0x0; break;
            case CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_HISTOGRAM_1518_E:
                value = 0x1; break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        if(useAsGePort)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                macRegs.perPortRegs[portNum].mibCountersCtrl;
            rc = prvCpssHwPpSetRegField(devNum, regAddr, 6, 1, value);
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                macRegs.perPortRegs[portNum].xgMibCountersCtrl;
            rc = prvCpssHwPpSetRegField(devNum, regAddr, 11, 1, value);
        }
    }
    else
    {
        rc = GT_BAD_PARAM;
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortMacOversizedPacketsCounterModeSet
*
* DESCRIPTION:
*       Sets MIB counters behaviour for oversized packets on given port.
*
* APPLICABLE DEVICES:
*        xCat2, xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number
*       counterMode - oversized packets counter mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number, device or counter mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The oversized packets counter mode determines the behavour of the
*       CPSS_PKTS_1024TOMAX_OCTETS_E and CPSS_DEFERRED_PKTS_SENT_E counters.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacOversizedPacketsCounterModeSet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_ENT  counterMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortMacOversizedPacketsCounterModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, counterMode));

    rc = internal_cpssDxChPortMacOversizedPacketsCounterModeSet(devNum, portNum, counterMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, counterMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortMacOversizedPacketsCounterModeGet
*
* DESCRIPTION:
*       Gets MIB counters behaviour for oversized packets on given port.
*
* APPLICABLE DEVICES:
*        xCat2; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       portNum        - physical port number
*
* OUTPUTS:
*       counterModePtr - (pointer to) oversized packets counter mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The oversized packets counter mode determines the behavour of the
*       CPSS_PKTS_1024TOMAX_OCTETS_E and CPSS_DEFERRED_PKTS_SENT_E counters.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortMacOversizedPacketsCounterModeGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    OUT CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_ENT  *counterModePtr
)
{
    GT_U32 regAddr;    /* register address to set */
    GT_U32 value;      /* value to set to bits */
    GT_U32 group;      /* Port's group (GOP) */
    GT_U32 mask;       /* bit's offset */
    GT_U32 portOffset; /* port's offset in register */
    GT_U32 useAsGePort;/* does this port uses the GE port mechanism or the XG mechanism */
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E| CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(counterModePtr);

    useAsGePort = IS_GE_PORT_COUNTERS_MAC(devNum,portNum);
    if(PRV_CPSS_PP_MAC(devNum)->appDevFamily == CPSS_XCAT2_E)
    {
        if(useAsGePort)
        {
            group = (PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum) / 6);
            if(group >= 4)
              CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);

            portOffset = (PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum) % 6);
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.macCountersCtrl[group] ;

            mask = 0x410000 << portOffset;

            rc = prvCpssDrvHwPpPortGroupReadRegBitMask(devNum,
              PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
              regAddr,mask,&value);
            if (rc != GT_OK)
            {
                return rc;
            }
            value >>= 16 + portOffset;
            switch (value & 0x41)
            {
                case 0x0:
                    *counterModePtr = CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_NORMAL_E;
                    break;
                case 0x1:
                    *counterModePtr = CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_HISTOGRAM_1518_E;
                    break;
                case 0x40:
                    *counterModePtr = CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_NORMAL_E;
                    break;
                case 0x41:
                    *counterModePtr = CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_HISTOGRAM_1522_E;
                    break;
            }
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
              macRegs.perPortRegs[portNum].xgMibCountersCtrl;
            rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,
              PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
              regAddr,6,2,&value);
            if (rc != GT_OK)
            {
                return rc;
            }
            switch (value)
            {
                case 0:
                    *counterModePtr = CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_NORMAL_E;
                    break;
                case 1:
                    *counterModePtr = CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_HISTOGRAM_1518_E;
                    break;
                case 2:
                    *counterModePtr = CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_NORMAL_E;
                    break;
                case 3:
                    *counterModePtr = CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_HISTOGRAM_1522_E;
                    break;
            }
        }
    }
    else if(PRV_CPSS_PP_MAC(devNum)->appDevFamily == CPSS_XCAT3_E)
    {
        if(useAsGePort)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                macRegs.perPortRegs[portNum].mibCountersCtrl;
            rc = prvCpssHwPpGetRegField(devNum, regAddr, 6, 1, &value);
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                macRegs.perPortRegs[portNum].xgMibCountersCtrl;
            rc = prvCpssHwPpGetRegField(devNum, regAddr, 11, 1, &value);
        }
        switch (value)
        {
            case 0:
                *counterModePtr = CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_NORMAL_E;
                break;
            case 1:
                *counterModePtr = CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_HISTOGRAM_1518_E;
                break;
        }
    }
    else
    {
        rc = GT_BAD_PARAM;
    }


    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortMacOversizedPacketsCounterModeGet
*
* DESCRIPTION:
*       Gets MIB counters behaviour for oversized packets on given port.
*
* APPLICABLE DEVICES:
*        xCat2; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       portNum        - physical port number
*
* OUTPUTS:
*       counterModePtr - (pointer to) oversized packets counter mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The oversized packets counter mode determines the behavour of the
*       CPSS_PKTS_1024TOMAX_OCTETS_E and CPSS_DEFERRED_PKTS_SENT_E counters.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacOversizedPacketsCounterModeGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    OUT CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_ENT  *counterModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortMacOversizedPacketsCounterModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, counterModePtr));

    rc = internal_cpssDxChPortMacOversizedPacketsCounterModeGet(devNum, portNum, counterModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, counterModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPortStatTxDebugCountersGet
*
* DESCRIPTION:
*       Gets egress TxQ drop counters.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - physical device number
*
* OUTPUTS:
*       dropCntrStcPtr  - (pointer to) drop counter structure
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortStatTxDebugCountersGet
(
    IN  GT_U8   devNum,
    OUT CPSS_DXCH_PORT_STAT_TX_DROP_COUNTERS_STC  *dropCntrStcPtr
)
{
    GT_STATUS rc;       /* return code */
    GT_U32 regAddr;     /* register address */
    GT_U32 regValue;    /* register value */
    GT_U32 i;           /* loop iterator */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(dropCntrStcPtr);

    regAddr = PRV_DXCH_REG1_UNIT_TXQ_DQ_MAC(devNum).global.dropCntrs.egrMirroredDroppedCntr;

    rc = prvCpssPortGroupsCounterSummary(devNum, regAddr, 0, 32, &regValue, NULL);
    if(GT_OK != rc)
    {
        return rc;
    }
    dropCntrStcPtr->egrMirrorDropCntr = regValue;

    regAddr = PRV_DXCH_REG1_UNIT_TXQ_DQ_MAC(devNum).global.dropCntrs.STCDroppedCntr;

    rc = prvCpssPortGroupsCounterSummary(devNum, regAddr, 0, 32, &regValue, NULL);
    if(GT_OK != rc)
    {
        return rc;
    }
    dropCntrStcPtr->egrStcDropCntr = regValue;

    regAddr = PRV_DXCH_REG1_UNIT_TXQ_QCN_MAC(devNum).CNDropCntr;

    rc = prvCpssPortGroupsCounterSummary(devNum, regAddr, 0, 32, &regValue, NULL);
    if(GT_OK != rc)
    {
        return rc;
    }
    dropCntrStcPtr->egrQcnDropCntr = regValue;

    regAddr = PRV_DXCH_REG1_UNIT_TXQ_PFC_MAC(devNum).PFCPortGroupFlowCtrlIndicationFIFOOverrunCntr[0];

    rc = prvCpssPortGroupsCounterSummary(devNum, regAddr, 0, 32, &regValue, NULL);
    if(GT_OK != rc)
    {
        return rc;
    }
    dropCntrStcPtr->dropPfcEventsCntr = regValue;


    dropCntrStcPtr->clearPacketsDroppedCounter = 0;
    for(i = 0; i < 4; i++)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_Q_MAC(devNum).global.dropCntrs.clearPktsDroppedCntrPipe[i];

        rc = prvCpssPortGroupsCounterSummary(devNum, regAddr, 0, 32, &regValue, NULL);
        if(GT_OK != rc)
        {
            return rc;
        }
        dropCntrStcPtr->clearPacketsDroppedCounter += regValue;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortStatTxDebugCountersGet
*
* DESCRIPTION:
*       Gets egress TxQ drop counters.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - physical device number
*
* OUTPUTS:
*       dropCntrStcPtr  - (pointer to) drop counter structure
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortStatTxDebugCountersGet
(
    IN  GT_U8   devNum,
    OUT CPSS_DXCH_PORT_STAT_TX_DROP_COUNTERS_STC  *dropCntrStcPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortStatTxDebugCountersGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, dropCntrStcPtr));

    rc = internal_cpssDxChPortStatTxDebugCountersGet(devNum, dropCntrStcPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, dropCntrStcPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


