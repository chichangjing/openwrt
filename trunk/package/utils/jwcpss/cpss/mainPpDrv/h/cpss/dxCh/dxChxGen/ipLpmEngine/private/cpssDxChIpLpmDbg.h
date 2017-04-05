/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChIpLpmDbg.h
*
* DESCRIPTION:
*       the CPSS DXCH debug .
*
* FILE REVISION NUMBER:
*       $Revision: 12 $
*******************************************************************************/

#ifndef __cpssDxChIpLpmDbgh
#define __cpssDxChIpLpmDbgh

#include <cpss/dxCh/dxChxGen/ipLpmEngine/private/cpssDxChPrvIpLpmTypes.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* dumpRouteTcam
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
*       GT_OK   - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_INITIALIZED       - if the driver was not initialized
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL - on failure.
*
* COMMENTS:
*     None
*
*******************************************************************************/
GT_STATUS dumpRouteTcam
(
    IN GT_BOOL dump
);

/*******************************************************************************
* cpssDxChIpPatTrieValidityCheck
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
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.
*       protocol    - ip protocol
*       prefixType   - uc/mc prefix type
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
GT_STATUS cpssDxChIpPatTrieValidityCheck
(
  IN GT_U32                     lpmDBId,
  IN GT_U32                     vrId,
  IN CPSS_IP_PROTOCOL_STACK_ENT protocol,
  IN CPSS_UNICAST_MULTICAST_ENT prefixType
);

/*******************************************************************************
* cpssDxChIpPatTriePrint
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
*     lpmDbId     - The LPM DB id.
*     vrId        - The virtual router identifier.
*     protocol    - ip protocol
*     prefixType  - uc/mc prefix type
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     GT_OK                    - on success
*     GT_NOT_FOUND             - if lpmDbId or vrId is not found
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChIpPatTriePrint
(
    IN GT_U32                     lpmDbId,
    IN GT_U32                     vrId,
    IN CPSS_IP_PROTOCOL_STACK_ENT protocol,
    IN CPSS_UNICAST_MULTICAST_ENT prefixType
);

/*******************************************************************************
* cpssDxChIpLpmHwValidation
*
* DESCRIPTION:
*   Validate the LPM in HW
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - The device number
*       vrId        - The virtual router id
*       protocol    - The IP protocol
*       prefixType  - Unicast or multicast
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChIpLpmHwValidation
(
    IN GT_U8                            devNum,
    IN GT_U32                           vrId,
    IN CPSS_IP_PROTOCOL_STACK_ENT       protocol,
    IN CPSS_UNICAST_MULTICAST_ENT       prefixType
);

/*******************************************************************************
* cpssDxChIpLpmShadowValidityCheck
*
* DESCRIPTION:
*   Validate the LPM in Shadow
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       lpmDbId         - The LPM DB id
*       vrId            - The virtual router id
*       protocol        - The IP protocol
*       prefixType      - Unicast or multicast
*       returnOnFailure - whether to return on first failure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChIpLpmShadowValidityCheck
(
    IN GT_U32                           lpmDbId,
    IN GT_U32                           vrId,
    IN CPSS_IP_PROTOCOL_STACK_ENT       protocol,
    IN CPSS_UNICAST_MULTICAST_ENT       prefixType,
    IN GT_BOOL                          returnOnFailure
);

/*******************************************************************************
* cpssDxChIpLpmHwShadowSyncValidityCheck
*
* DESCRIPTION:
*   This function validates synchronization between the SW and HW of the LPM
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - The device number
*       lpmDbId         - The LPM DB id
*       vrId            - The virtual router id
*       protocol        - The IP protocol
*       prefixType      - Unicast or multicast
*       returnOnFailure - whether to return on first failure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChIpLpmHwShadowSyncValidityCheck
(
    IN GT_U8                            devNum,
    IN GT_U32                           lpmDbId,
    IN GT_U32                           vrId,
    IN CPSS_IP_PROTOCOL_STACK_ENT       protocol,
    IN CPSS_UNICAST_MULTICAST_ENT       prefixType,
    IN GT_BOOL                          returnOnFailure
);

/*******************************************************************************
* cpssDxChIpLpmHwShadowValidityAndSyncCheck
*
* DESCRIPTION:
*   This function Validate the LPM in Shadow, Validate the LPM in HW and
*   check synchronization between the SW and HW of the LPM
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - The device number
*       lpmDbId         - The LPM DB id
*       vrId            - The virtual router id
*       protocol        - The IP protocol
*       prefixType      - Unicast or multicast
*       returnOnFailure - whether to return on first failure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChIpLpmHwShadowValidityAndSyncCheck
(
    IN GT_U8                            devNum,
    IN GT_U32                           lpmDbId,
    IN GT_U32                           vrId,
    IN CPSS_IP_PROTOCOL_STACK_ENT       protocol,
    IN CPSS_UNICAST_MULTICAST_ENT       prefixType,
    IN GT_BOOL                          returnOnFailure
);

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixAddMany
*
* DESCRIPTION:
*       This debug function tries to add many sequential IPv4 Unicast prefixes and
*       returns the number of prefixes successfully added.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       startIpAddr           - the first address to add
*       routeEntryBaseMemAddr - base memory Address (offset) of the route entry
*       numOfPrefixesToAdd    - the number of prefixes to add
*
* OUTPUTS:
*       lastIpAddrAddedPtr    - points to the last prefix successfully
*                               added (NULL to ignore)
*       numOfPrefixesAddedPtr - points to the nubmer of prefixes that were
*                               successfully added (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that can be added. All the prefixes are added with exact match (prefix
*       length 32). The route entry is not written.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixAddMany
(
    IN  GT_U32      lpmDbId,
    IN  GT_U32      vrId,
    IN  GT_IPADDR   startIpAddr,
    IN  GT_U32      routeEntryBaseMemAddr,
    IN  GT_U32      numOfPrefixesToAdd,
    OUT GT_IPADDR   *lastIpAddrAddedPtr,
    OUT GT_U32      *numOfPrefixesAddedPtr
);

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixAddManyRandom
*
* DESCRIPTION:
*       This function tries to add many random IPv4 Unicast prefixes and
*       returns the number of prefixes successfully added.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       startIpAddr           - the first address to add
*       routeEntryBaseMemAddr - base memory Address (offset) of the route entry
*       numOfPrefixesToAdd    - the number of prefixes to add
*       isWholeIpRandom       - GT_TRUE: all IP octets calculated by cpssOsRand
*                               GT_FALSE: only 2 LSB octets calculated by cpssOsRand
*
* OUTPUTS:
*       numOfPrefixesAddedPtr - points to the nubmer of prefixes that were
*                               successfully added (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that can be added. All the prefixes are added with exact match (prefix
*       length 32). The route entry is not written.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixAddManyRandom
(
    IN  GT_U32      lpmDbId,
    IN  GT_U32      vrId,
    IN  GT_IPADDR   startIpAddr,
    IN  GT_U32      routeEntryBaseMemAddr,
    IN  GT_U32      numOfPrefixesToAdd,
    IN  GT_BOOL     isWholeIpRandom,
    OUT GT_U32      *numOfPrefixesAddedPtr
);

/*******************************************************************************
* cpssDxChIpLpmDump
*
* DESCRIPTION:
*   Print hardware LPM dump
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - The device number
*       vrId        - The virtual router id
*       protocol    - The IP protocol
*       prefixType  - Unicast or multicast
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDump
(
    IN GT_U8                            devNum,
    IN GT_U32                           vrId,
    IN CPSS_IP_PROTOCOL_STACK_ENT       protocol,
    IN CPSS_UNICAST_MULTICAST_ENT       prefixType
);

/*******************************************************************************
* cpssDxChIpLpmDbHwMemPrint
*
* DESCRIPTION:
*   Print DMM partition debug information for a specific LPM DB
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
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDbHwMemPrint
(
    IN GT_U32                           lpmDbId
);



/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixAddManyByOctet
*
* DESCRIPTION:
*       This function tries to add many sequential IPv4 Unicast prefixes according
*       to octet order selection and returns the number of prefixes successfully added.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       startIpAddr           - the first address to add
*       routeEntryBaseMemAddr - base memory Address (offset) of the route entry
*       numOfPrefixesToAdd    - the number of prefixes to add
*       firstOctetToChange    - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       secondOctetToChange   - second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       thirdOctetToChange    - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       forthOctetToChange    - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*
* OUTPUTS:
*       lastIpAddrAddedPtr    - points to the last prefix successfully
*                               added (NULL to ignore)
*       numOfPrefixesAddedPtr - points to the nubmer of prefixes that were
*                               successfully added (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that can be added. All the prefixes are added with exact match (prefix
*       length 32). The route entry is not written.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixAddManyByOctet
(
    IN  GT_U32      lpmDbId,
    IN  GT_U32      vrId,
    IN  GT_IPADDR   startIpAddr,
    IN  GT_U32      routeEntryBaseMemAddr,
    IN  GT_U32      numOfPrefixesToAdd,
    IN  GT_U32      firstOctetToChange,
    IN  GT_U32      secondOctetToChange,
    IN  GT_U32      thirdOctetToChange,
    IN  GT_U32      forthOctetToChange,
    OUT GT_IPADDR   *lastIpAddrAddedPtr,
    OUT GT_U32      *numOfPrefixesAddedPtr
);

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixDelManyByOctet
*
* DESCRIPTION:
*       This function tries to delete many sequential IPv4 Unicast prefixes according
*       to octet order selection and returns the number of prefixes successfully deleted.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       startIpAddr           - the first address to add
*       numOfPrefixesToDel    - the number of prefixes to delete
*       firstOctetToChange    - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       secondOctetToChange   - second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       thirdOctetToChange    - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       forthOctetToChange    - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*
* OUTPUTS:
*       lastIpAddrDeletedPtr    - points to the last prefix successfully
*                                 deleted (NULL to ignore)
*       numOfPrefixesDeletedPtr - points to the nubmer of prefixes that were
*                                 successfully deleted (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to delete the number of the prefixes
*       that was added by cpssDxChIpLpmIpv4UcPrefixAddManyByOctet.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixDelManyByOctet
(
    IN  GT_U32      lpmDbId,
    IN  GT_U32      vrId,
    IN  GT_IPADDR   startIpAddr,
    IN  GT_U32      numOfPrefixesToDel,
    IN  GT_U32      firstOctetToChange,
    IN  GT_U32      secondOctetToChange,
    IN  GT_U32      thirdOctetToChange,
    IN  GT_U32      forthOctetToChange,
    OUT GT_IPADDR   *lastIpAddrDeletedPtr,
    OUT GT_U32      *numOfPrefixesDeletedPtr
);

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixGetManyByOctet
*
* DESCRIPTION:
*       This function tries to get many sequential IPv4 Unicast prefixes (that were
*       added by cpssDxChIpLpmIpv4UcPrefixAddManyByOctet) and returns the number of
*       successfully retrieved prefixes.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       startIpAddr           - the first address to add
*       numOfPrefixesToGet    - the number of prefixes to get
*       firstOctetToChange    - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       secondOctetToChange   - second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       thirdOctetToChange    - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       forthOctetToChange    - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*
* OUTPUTS:
*       numOfRetrievedPrefixesPtr - points to the nubmer of prefixes that were
*                                   successfully retrieved (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that were be added.
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixGetManyByOctet
(
    IN  GT_U32      lpmDbId,
    IN  GT_U32      vrId,
    IN  GT_IPADDR   startIpAddr,
    IN  GT_U32      numOfPrefixesToGet,
    IN  GT_U32      firstOctetToChange,
    IN  GT_U32      secondOctetToChange,
    IN  GT_U32      thirdOctetToChange,
    IN  GT_U32      forthOctetToChange,
    OUT GT_U32      *numOfRetrievedPrefixesPtr
);

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixAddManyByOctet
*
* DESCRIPTION:
*       This function tries to add many sequential IPv6 Unicast prefixes according
*       to octet order selection and returns the number of prefixes successfully added.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       startIpAddr           - the first address to add
*       routeEntryBaseMemAddr - base memory Address (offset) of the route entry
*       numOfPrefixesToAdd    - the number of prefixes to add
*       firstOctetToChange    - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       secondOctetToChange   - second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       thirdOctetToChange    - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       forthOctetToChange    - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       fifthOctetToChange    - fifth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       sixthOctetToChange    - sixth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       seventhOctetToChange  - seventh octet to change in the prefix address.
*                               value of the octet can be up to 255
*       eighthOctetToChange   - eighth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       ninthOctetToChange    - ninth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       tenthOctetToChange    - tenth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       elevenOctetToChange   - eleven octet to change in the prefix address.
*                               value of the octet can be up to 255
*       twelveOctetToChange   - twelve octet to change in the prefix address.
*                               value of the octet can be up to 255
*       thirteenOctetToChange - thirteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       fourteenOctetToChange - fourteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       fifteenOctetToChange  - fifteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       sixteenOctetToChange  - sixteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*
* OUTPUTS:
*       lastIpAddrAddedPtr    - points to the last prefix successfully
*                               added (NULL to ignore)
*       numOfPrefixesAddedPtr - points to the nubmer of prefixes that were
*                               successfully added (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that can be added. All the prefixes are added with exact match (prefix
*       length 128). The route entry is not written.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixAddManyByOctet
(
    IN  GT_U32      lpmDbId,
    IN  GT_U32      vrId,
    IN  GT_IPV6ADDR startIpAddr,
    IN  GT_U32      routeEntryBaseMemAddr,
    IN  GT_U32      numOfPrefixesToAdd,
    IN  GT_U32      firstOctetToChange,
    IN  GT_U32      secondOctetToChange,
    IN  GT_U32      thirdOctetToChange,
    IN  GT_U32      forthOctetToChange,
    IN  GT_U32      fifthOctetToChange,
    IN  GT_U32      sixthOctetToChange,
    IN  GT_U32      seventhOctetToChange,
    IN  GT_U32      eighthOctetToChange,
    IN  GT_U32      ninthOctetToChange,
    IN  GT_U32      tenthOctetToChange,
    IN  GT_U32      elevenOctetToChange,
    IN  GT_U32      twelveOctetToChange,
    IN  GT_U32      thirteenOctetToChange,
    IN  GT_U32      fourteenOctetToChange,
    IN  GT_U32      fifteenOctetToChange,
    IN  GT_U32      sixteenOctetToChange,
    OUT GT_IPV6ADDR *lastIpAddrAddedPtr,
    OUT GT_U32      *numOfPrefixesAddedPtr
);

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixDelManyByOctet
*
* DESCRIPTION:
*       This function tries to delete many sequential IPv6 Unicast prefixes according
*       to octet order selection and returns the number of prefixes successfully deleted.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       startIpAddr           - the first address to add
*       numOfPrefixesToDel    - the number of prefixes to deleted
*       firstOctetToChange    - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       secondOctetToChange   - second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       thirdOctetToChange    - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       forthOctetToChange    - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       fifthOctetToChange    - fifth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       sixthOctetToChange    - sixth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       seventhOctetToChange  - seventh octet to change in the prefix address.
*                               value of the octet can be up to 255
*       eighthOctetToChange   - eighth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       ninthOctetToChange    - ninth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       tenthOctetToChange    - tenth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       elevenOctetToChange   - eleven octet to change in the prefix address.
*                               value of the octet can be up to 255
*       twelveOctetToChange   - twelve octet to change in the prefix address.
*                               value of the octet can be up to 255
*       thirteenOctetToChange - thirteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       fourteenOctetToChange - fourteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       fifteenOctetToChange  - fifteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       sixteenOctetToChange  - sixteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*
* OUTPUTS:
*       lastIpAddrDeletedPtr    - points to the last prefix successfully
*                               deleted (NULL to ignore)
*       numOfPrefixesDeletedPtr - points to the nubmer of prefixes that were
*                               successfully deleted (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to delete the number of prefixes
*       that was added by cpssDxChIpLpmIpv6UcPrefixAddManyByOctet.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixDelManyByOctet
(
    IN  GT_U32      lpmDbId,
    IN  GT_U32      vrId,
    IN  GT_IPV6ADDR startIpAddr,
    IN  GT_U32      numOfPrefixesToDel,
    IN  GT_U32      firstOctetToChange,
    IN  GT_U32      secondOctetToChange,
    IN  GT_U32      thirdOctetToChange,
    IN  GT_U32      forthOctetToChange,
    IN  GT_U32      fifthOctetToChange,
    IN  GT_U32      sixthOctetToChange,
    IN  GT_U32      seventhOctetToChange,
    IN  GT_U32      eighthOctetToChange,
    IN  GT_U32      ninthOctetToChange,
    IN  GT_U32      tenthOctetToChange,
    IN  GT_U32      elevenOctetToChange,
    IN  GT_U32      twelveOctetToChange,
    IN  GT_U32      thirteenOctetToChange,
    IN  GT_U32      fourteenOctetToChange,
    IN  GT_U32      fifteenOctetToChange,
    IN  GT_U32      sixteenOctetToChange,
    OUT GT_IPV6ADDR *lastIpAddrAddedPtr,
    OUT GT_U32      *numOfPrefixesDeletedPtr
);

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixGetManyByOctet
*
* DESCRIPTION:
*       This function tries to get many sequential IPv6 Unicast prefixes (that were
*       added by cpssDxChIpLpmIpv6UcPrefixAddManyByOctet) and returns the number of
*       successfully retrieved prefixes.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       startIpAddr           - the first address to add
*       numOfPrefixesToGet    - the number of prefixes to get
*       firstOctetToChange    - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       secondOctetToChange   - second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       thirdOctetToChange    - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       forthOctetToChange    - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       fifthOctetToChange    - fifth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       sixthOctetToChange    - sixth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       seventhOctetToChange  - seventh octet to change in the prefix address.
*                               value of the octet can be up to 255
*       eighthOctetToChange   - eighth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       ninthOctetToChange    - ninth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       tenthOctetToChange    - tenth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       elevenOctetToChange   - eleven octet to change in the prefix address.
*                               value of the octet can be up to 255
*       twelveOctetToChange   - twelve octet to change in the prefix address.
*                               value of the octet can be up to 255
*       thirteenOctetToChange - thirteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       fourteenOctetToChange - fourteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       fifteenOctetToChange  - fifteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       sixteenOctetToChange  - sixteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*
* OUTPUTS:
*       numOfRetrievedPrefixesPtr - points to the nubmer of prefixes that were
*                                   successfully retrieved (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that were be added.
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixGetManyByOctet
(
    IN  GT_U32      lpmDbId,
    IN  GT_U32      vrId,
    IN  GT_IPV6ADDR startIpAddr,
    IN  GT_U32      numOfPrefixesToGet,
    IN  GT_U32      firstOctetToChange,
    IN  GT_U32      secondOctetToChange,
    IN  GT_U32      thirdOctetToChange,
    IN  GT_U32      forthOctetToChange,
    IN  GT_U32      fifthOctetToChange,
    IN  GT_U32      sixthOctetToChange,
    IN  GT_U32      seventhOctetToChange,
    IN  GT_U32      eighthOctetToChange,
    IN  GT_U32      ninthOctetToChange,
    IN  GT_U32      tenthOctetToChange,
    IN  GT_U32      elevenOctetToChange,
    IN  GT_U32      twelveOctetToChange,
    IN  GT_U32      thirteenOctetToChange,
    IN  GT_U32      fourteenOctetToChange,
    IN  GT_U32      fifteenOctetToChange,
    IN  GT_U32      sixteenOctetToChange,
    OUT GT_U32      *numOfRetrievedPrefixesPtr
);


/*******************************************************************************
* cpssDxChIpLpmIpv4McPrefixAddManyByOctet
*
* DESCRIPTION:
*       This function tries to add many sequential IPv4 Multicast prefixes according
*       to octet order selection and returns the number of prefixes successfully added.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       grpStartIpAddr        - the first group address to add
*       srcStartIpAddr        - the first source address to add
*       routeEntryBaseMemAddr - base memory Address (offset) of the route entry
*       numOfPrefixesToAdd    - the number of prefixes to add
*       changeGrpAddr         - GT_TRUE: change the group address
*                               GT_FALSE: don't change the group address
*       grpFirstOctetToChange - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpSecondOctetToChange- second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpThirdOctetToChange - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpForthOctetToChange - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       changeSrcAddr         - GT_TRUE: change the source address
*                               GT_FALSE: don't change the source address
*       srcFirstOctetToChange - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcSecondOctetToChange- second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcThirdOctetToChange - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcForthOctetToChange - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*
* OUTPUTS:
*       grpLastIpAddrAddedPtr - points to the last group prefix successfully
*                               added (NULL to ignore)
*       srcLastIpAddrAddedPtr - points to the last source prefix successfully
*                               added (NULL to ignore)
*       numOfPrefixesAddedPtr - points to the nubmer of prefixes that were
*                               successfully added (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that can be added. All the prefixes are added with exact match (prefix
*       length 32). The route entry is not written.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4McPrefixAddManyByOctet
(
    IN  GT_U32      lpmDbId,
    IN  GT_U32      vrId,
    IN  GT_IPADDR   grpStartIpAddr,
    IN  GT_IPADDR   srcStartIpAddr,
    IN  GT_U32      routeEntryBaseMemAddr,
    IN  GT_U32      numOfPrefixesToAdd,
    IN  GT_BOOL     changeGrpAddr,
    IN  GT_U32      grpFirstOctetToChange,
    IN  GT_U32      grpSecondOctetToChange,
    IN  GT_U32      grpThirdOctetToChange,
    IN  GT_U32      grpForthOctetToChange,
    IN  GT_BOOL     changeSrcAddr,
    IN  GT_U32      srcFirstOctetToChange,
    IN  GT_U32      srcSecondOctetToChange,
    IN  GT_U32      srcThirdOctetToChange,
    IN  GT_U32      srcForthOctetToChange,
    OUT GT_IPADDR   *grpLastIpAddrAddedPtr,
    OUT GT_IPADDR   *srcLastIpAddrAddedPtr,
    OUT GT_U32      *numOfPrefixesAddedPtr
);

/*******************************************************************************
* cpssDxChIpLpmIpv4McPrefixGetManyByOctet
*
* DESCRIPTION:
*       This function tries to get many sequential IPv4 Multicast prefixes (that were
*       added by cpssDxChIpLpmIpv4McPrefixAddManyByOctet) and returns the number of
*       successfully retrieved prefixes.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       grpStartIpAddr        - the first group address to add
*       srcStartIpAddr        - the first source address to add
*       numOfPrefixesToGet    - the number of prefixes to get
*       changeGrpAddr         - GT_TRUE: change the group address
*                               GT_FALSE: don't change the group address
*       grpFirstOctetToChange - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpSecondOctetToChange- second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpThirdOctetToChange - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpForthOctetToChange - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       changeSrcAddr         - GT_TRUE: change the source address
*                               GT_FALSE: don't change the source address
*       srcFirstOctetToChange - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcSecondOctetToChange- second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcThirdOctetToChange - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcForthOctetToChange - forth octet to change in the prefix address.
*                               value of the octet can be up to 255*
* OUTPUTS:
*       numOfRetrievedPrefixesPtr - points to the nubmer of prefixes that were
*                                   successfully retrieved (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that were be added.
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4McPrefixGetManyByOctet
(
    IN  GT_U32      lpmDbId,
    IN  GT_U32      vrId,
    IN  GT_IPADDR   grpStartIpAddr,
    IN  GT_IPADDR   srcStartIpAddr,
    IN  GT_U32      numOfPrefixesToGet,
    IN  GT_BOOL     changeGrpAddr,
    IN  GT_U32      grpFirstOctetToChange,
    IN  GT_U32      grpSecondOctetToChange,
    IN  GT_U32      grpThirdOctetToChange,
    IN  GT_U32      grpForthOctetToChange,
    IN  GT_BOOL     changeSrcAddr,
    IN  GT_U32      srcFirstOctetToChange,
    IN  GT_U32      srcSecondOctetToChange,
    IN  GT_U32      srcThirdOctetToChange,
    IN  GT_U32      srcForthOctetToChange,
    OUT GT_U32      *numOfRetrievedPrefixesPtr
);

/*******************************************************************************
* cpssDxChIpLpmIpv6McPrefixAddManyByOctet
*
* DESCRIPTION:
*       This function tries to add many sequential IPv6 Multicast prefixes according
*       to octet order selection and returns the number of prefixes successfully added.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       grpStartIpAddr        - the first group address to add
*       ipGroupRuleIndex      - index of the IPv6 mc group rule in the TCAM.
*       srcStartIpAddr        - the first source address to add
*       routeEntryBaseMemAddr - base memory Address (offset) of the route entry
*       numOfPrefixesToAdd    - the number of prefixes to add
*       changeGrpAddr         - GT_TRUE: change the group address
*                               GT_FALSE: don't change the group address
*       grpFirstOctetToChange - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpSecondOctetToChange- second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpThirdOctetToChange - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpForthOctetToChange - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpFifthOctetToChange - fifth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpSixthOctetToChange - sixth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpSeventhOctetToChange  - seventh octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpEighthOctetToChange- eighth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpNinthOctetToChange - ninth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpTenthOctetToChange - tenth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpElevenOctetToChange- eleven octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpTwelveOctetToChange- twelve octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpThirteenOctetToChange - thirteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpFourteenOctetToChange - fourteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpFifteenOctetToChange  - fifteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpSixteenOctetToChange  - sixteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       changeSrcAddr         - GT_TRUE: change the source address
*                               GT_FALSE: don't change the source address
*       srcFirstOctetToChange - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcSecondOctetToChange- second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcThirdOctetToChange - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcForthOctetToChange - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcFifthOctetToChange - fifth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcSixthOctetToChange - sixth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcSeventhOctetToChange  - seventh octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcEighthOctetToChange- eighth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcNinthOctetToChange - ninth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcTenthOctetToChange - tenth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcElevenOctetToChange- eleven octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcTwelveOctetToChange- twelve octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcThirteenOctetToChange - thirteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcFourteenOctetToChange - fourteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcFifteenOctetToChange  - fifteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcSixteenOctetToChange  - sixteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*
* OUTPUTS:
*       grpLastIpAddrAddedPtr - points to the last group prefix successfully
*                               added (NULL to ignore)
*       srcLastIpAddrAddedPtr - points to the last source prefix successfully
*                               added (NULL to ignore)
*       numOfPrefixesAddedPtr - points to the nubmer of prefixes that were
*                               successfully added (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that can be added. All the prefixes are added with exact match (prefix
*       length 128). The route entry is not written.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6McPrefixAddManyByOctet
(
    IN  GT_U32      lpmDbId,
    IN  GT_U32      vrId,
    IN  GT_IPV6ADDR grpStartIpAddr,
    IN  GT_IPV6ADDR srcStartIpAddr,
    IN  GT_U32      routeEntryBaseMemAddr,
    IN  GT_U32      numOfPrefixesToAdd,
    IN  GT_BOOL     changeGrpAddr,
    IN  GT_U32      grpFirstOctetToChange,
    IN  GT_U32      grpSecondOctetToChange,
    IN  GT_U32      grpThirdOctetToChange,
    IN  GT_U32      grpForthOctetToChange,
    IN  GT_U32      grpFifthOctetToChange,
    IN  GT_U32      grpSixthOctetToChange,
    IN  GT_U32      grpSeventhOctetToChange,
    IN  GT_U32      grpEighthOctetToChange,
    IN  GT_U32      grpNinthOctetToChange,
    IN  GT_U32      grpTenthOctetToChange,
    IN  GT_U32      grpElevenOctetToChange,
    IN  GT_U32      grpTwelveOctetToChange,
    IN  GT_U32      grpThirteenOctetToChange,
    IN  GT_U32      grpFourteenOctetToChange,
    IN  GT_U32      grpFifteenOctetToChange,
    IN  GT_U32      grpSixteenOctetToChange,
    IN  GT_BOOL     changeSrcAddr,
    IN  GT_U32      srcFirstOctetToChange,
    IN  GT_U32      srcSecondOctetToChange,
    IN  GT_U32      srcThirdOctetToChange,
    IN  GT_U32      srcForthOctetToChange,
    IN  GT_U32      srcFifthOctetToChange,
    IN  GT_U32      srcSixthOctetToChange,
    IN  GT_U32      srcSeventhOctetToChange,
    IN  GT_U32      srcEighthOctetToChange,
    IN  GT_U32      srcNinthOctetToChange,
    IN  GT_U32      srcTenthOctetToChange,
    IN  GT_U32      srcElevenOctetToChange,
    IN  GT_U32      srcTwelveOctetToChange,
    IN  GT_U32      srcThirteenOctetToChange,
    IN  GT_U32      srcFourteenOctetToChange,
    IN  GT_U32      srcFifteenOctetToChange,
    IN  GT_U32      srcSixteenOctetToChange,
    OUT GT_IPV6ADDR *grpLastIpAddrAddedPtr,
    OUT GT_IPV6ADDR *srcLastIpAddrAddedPtr,
    OUT GT_U32      *numOfPrefixesAddedPtr
);

/*******************************************************************************
* cpssDxChIpLpmIpv6McPrefixGetManyByOctet
*
* DESCRIPTION:
*       This function tries to get many sequential IPv6 Multicast prefixes (that were
*       added by cpssDxChIpLpmIpv6McPrefixAddManyByOctet) and returns the number of
*       successfully retrieved prefixes.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       grpStartIpAddr        - the first group address to add
*       srcStartIpAddr        - the first source address to add
*       numOfPrefixesToGet    - the number of prefixes to get
*       changeGrpAddr         - GT_TRUE: change the group address
*                               GT_FALSE: don't change the group address
*       grpFirstOctetToChange - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpSecondOctetToChange- second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpThirdOctetToChange - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpForthOctetToChange - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpFifthOctetToChange - fifth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpSixthOctetToChange - sixth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpSeventhOctetToChange  - seventh octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpEighthOctetToChange- eighth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpNinthOctetToChange - ninth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpTenthOctetToChange - tenth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpElevenOctetToChange- eleven octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpTwelveOctetToChange- twelve octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpThirteenOctetToChange - thirteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpFourteenOctetToChange - fourteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpFifteenOctetToChange  - fifteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpSixteenOctetToChange  - sixteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       changeSrcAddr         - GT_TRUE: change the source address
*                               GT_FALSE: don't change the source address
*       srcFirstOctetToChange - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcSecondOctetToChange- second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcThirdOctetToChange - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcForthOctetToChange - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcFifthOctetToChange - fifth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcSixthOctetToChange - sixth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcSeventhOctetToChange  - seventh octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcEighthOctetToChange- eighth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcNinthOctetToChange - ninth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcTenthOctetToChange - tenth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcElevenOctetToChange- eleven octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcTwelveOctetToChange- twelve octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcThirteenOctetToChange - thirteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcFourteenOctetToChange - fourteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcFifteenOctetToChange  - fifteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcSixteenOctetToChange  - sixteen octet to change in the prefix address.
*                               value of the octet can be up to 255
* OUTPUTS:
*       numOfRetrievedPrefixesPtr - points to the nubmer of prefixes that were
*                                   successfully retrieved (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that were be added.
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6McPrefixGetManyByOctet
(
    IN  GT_U32      lpmDbId,
    IN  GT_U32      vrId,
    IN  GT_IPV6ADDR grpStartIpAddr,
    IN  GT_IPV6ADDR srcStartIpAddr,
    IN  GT_U32      numOfPrefixesToGet,
    IN  GT_BOOL     changeGrpAddr,
    IN  GT_U32      grpFirstOctetToChange,
    IN  GT_U32      grpSecondOctetToChange,
    IN  GT_U32      grpThirdOctetToChange,
    IN  GT_U32      grpForthOctetToChange,
    IN  GT_U32      grpFifthOctetToChange,
    IN  GT_U32      grpSixthOctetToChange,
    IN  GT_U32      grpSeventhOctetToChange,
    IN  GT_U32      grpEighthOctetToChange,
    IN  GT_U32      grpNinthOctetToChange,
    IN  GT_U32      grpTenthOctetToChange,
    IN  GT_U32      grpElevenOctetToChange,
    IN  GT_U32      grpTwelveOctetToChange,
    IN  GT_U32      grpThirteenOctetToChange,
    IN  GT_U32      grpFourteenOctetToChange,
    IN  GT_U32      grpFifteenOctetToChange,
    IN  GT_U32      grpSixteenOctetToChange,
    IN  GT_BOOL     changeSrcAddr,
    IN  GT_U32      srcFirstOctetToChange,
    IN  GT_U32      srcSecondOctetToChange,
    IN  GT_U32      srcThirdOctetToChange,
    IN  GT_U32      srcForthOctetToChange,
    IN  GT_U32      srcFifthOctetToChange,
    IN  GT_U32      srcSixthOctetToChange,
    IN  GT_U32      srcSeventhOctetToChange,
    IN  GT_U32      srcEighthOctetToChange,
    IN  GT_U32      srcNinthOctetToChange,
    IN  GT_U32      srcTenthOctetToChange,
    IN  GT_U32      srcElevenOctetToChange,
    IN  GT_U32      srcTwelveOctetToChange,
    IN  GT_U32      srcThirteenOctetToChange,
    IN  GT_U32      srcFourteenOctetToChange,
    IN  GT_U32      srcFifteenOctetToChange,
    IN  GT_U32      srcSixteenOctetToChange,
    OUT GT_U32      *numOfRetrievedPrefixesPtr
);

/*******************************************************************************
* cpssDxChIpLpmDbgHwOctetPerBlockPrint
*
* DESCRIPTION:
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
*       GT_OK                    - on success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_FOUND             - if can't find the lpm DB
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDbgHwOctetPerBlockPrint
(
    IN GT_U32                           lpmDbId
);

/*******************************************************************************
* cpssDxChIpLpmDbgHwOctetPerProtocolPrint
*
* DESCRIPTION:
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
*       GT_OK                    - on success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_FOUND             - if can't find the lpm DB
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDbgHwOctetPerProtocolPrint
(
    IN GT_U32                           lpmDbId
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChIpLpmDbgh */

