/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChIp.h
*
* DESCRIPTION:
*       The CPSS DXCH Ip HW structures APIs
*
* FILE REVISION NUMBER:
*       $Revision: 22 $
*******************************************************************************/
#ifndef __cpssDxChIph
#define __cpssDxChIph

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpTypes.h>


/*******************************************************************************
* cpssDxChIpv4PrefixSet
*
* DESCRIPTION:
*    Sets an ipv4 UC or MC prefix to the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to set
*       routerTtiTcamColumn       - the TCAM column to set
*       prefixPtr                 - prefix to set
*       maskPtr                   - mask of the prefix
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on fail.
*       GT_BAD_PARAM    - on devNum not active
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   1.  IP prefix entries reside in router TCAM. The router TCAM is organized
*       in rows, where each row contains 4 column entries for Ch3 and 5 column
*       entries for Ch2. Each line can hold:
*       - 1 TTI (tunnel termination interface) entry
*       - 1 ipv6 address
*       - 4 (ch3)/ 5 (ch2) ipv4 addresses
*       Note that if a line is used for ipv6 address or TTI entry then
*       it can't be used for ipv4 addresses and if an entry in a line is used
*       for ipv4 address, then the other line entries must hold ipv4 addresses
*       as well.
*   2.  The match for prefixes is done using a first match , thus if two prefixes
*       can match a packet (they have the same prefix , but diffrent prefix
*       length) and we want LPM to work on them we have to make sure to put
*       the longest prefix before the short one.
*       Search order is by columns, first column, then second and so on.
*       All and all default UC prefix 0.0.0.0/mask 0 must reside at the last
*       possible ipv4 prefix, and default MC prefix 224.0.0.0/ mask 240.0.0.0
*       must reside at the index before it.
*   3.  In Ch2 MC source addresses can reside only on the second and third
*       columns (on other PP, there is no such restriction).
*   4.  In Ch2 there is no support for multiple virtual routers. Therefore in
*       Ch2 vrId is ignored.
*
*******************************************************************************/
GT_STATUS cpssDxChIpv4PrefixSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerTtiTcamRow,
    IN  GT_U32                      routerTtiTcamColumn,
    IN  CPSS_DXCH_IPV4_PREFIX_STC   *prefixPtr,
    IN  CPSS_DXCH_IPV4_PREFIX_STC   *maskPtr
);


/*******************************************************************************
* cpssDxChIpv4PrefixGet
*
* DESCRIPTION:
*    Gets an ipv4 UC or MC prefix from the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to get from
*       routerTtiTcamColumn       - the TCAM column to get from
*
* OUTPUTS:
*       validPtr              - whether the entry is valid (if the entry isn't valid
*                               all the following fields will not get values)
*       prefixPtr             - prefix to set
*       maskPtr               - mask of the prefix
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpv4PrefixSet
*
*******************************************************************************/
GT_STATUS cpssDxChIpv4PrefixGet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerTtiTcamRow,
    IN  GT_U32                      routerTtiTcamColumn,
    OUT GT_BOOL                     *validPtr,
    OUT CPSS_DXCH_IPV4_PREFIX_STC   *prefixPtr,
    OUT CPSS_DXCH_IPV4_PREFIX_STC   *maskPtr
);


/*******************************************************************************
* cpssDxChIpv4PrefixInvalidate
*
* DESCRIPTION:
*    Invalidates an ipv4 UC or MC prefix in the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to invalidate
*       routerTtiTcamColumn       - the TCAM column to invalidate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpv4PrefixInvalidate
(
    IN  GT_U8       devNum,
    IN  GT_U32      routerTtiTcamRow,
    IN  GT_U32      routerTtiTcamColumn
);

/*******************************************************************************
* cpssDxChIpv6PrefixSet
*
* DESCRIPTION:
*    Sets an ipv6 UC or MC prefix to the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to set
*       prefixPtr                 - prefix to set
*       maskPtr                   - mask of the prefix
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpv4PrefixSet
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpv6PrefixSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerTtiTcamRow,
    IN  CPSS_DXCH_IPV6_PREFIX_STC   *prefixPtr,
    IN  CPSS_DXCH_IPV6_PREFIX_STC   *maskPtr
);

/*******************************************************************************
* cpssDxChIpv6PrefixGet
*
* DESCRIPTION:
*    Gets an ipv6 UC or MC prefix from the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to get from
*
* OUTPUTS:
*       validPtr              - whether the entry is valid (if the entry isn't valid
*                               all the following fields will not get values)
*       prefixPtr             - prefix to set
*       maskPtr               - mask of the prefix
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpv6PrefixSet
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpv6PrefixGet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerTtiTcamRow,
    OUT GT_BOOL                     *validPtr,
    OUT CPSS_DXCH_IPV6_PREFIX_STC   *prefixPtr,
    OUT CPSS_DXCH_IPV6_PREFIX_STC   *maskPtr
);

/*******************************************************************************
* cpssDxChIpv6PrefixInvalidate
*
* DESCRIPTION:
*    Invalidates an ipv6 UC or MC prefix in the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to invalidate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on devNum not active.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Note that invalidating ipv6 address (that takes whole TCAM line) will
*       result invalidating all columns in that line.
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpv6PrefixInvalidate
(
    IN  GT_U8           devNum,
    IN  GT_U32          routerTtiTcamRow
);


/*******************************************************************************
* cpssDxChIpLttWrite
*
* DESCRIPTION:
*    Writes a LookUp Translation Table Entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - the device number.
*       lttTtiRow     - the entry's row index in LTT table (equivalent to
*                       the router tcam entry it is attached to)
*       lttTtiColumn   - the entry's column index in LTT table (equivalent to
*                       the router tcam entry it is attached to)
*       lttEntryPtr   - the lookup translation table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - if succeeded
*       GT_BAD_PARAM    - on devNum not active or
*                         invalid ipv6MCGroupScopeLevel value.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Lookup translation table is shared with the Tunnel Termination (TT)
*       Table.
*       If the relevant index in the Router Tcam is in line used for tunnel
*       termination key then the indexes in the LTT/TT table will function as
*       Tunnel Termination Action, and you shouldn't write LTT entries in these
*       indexes. It works Vice-verse.
*       For Ipv6 Entries the relevant index is the line prefix (same index sent
*       as parameter to the ipv6 prefix set API). The other LTT/TT entries
*       related to that TCAM line are left unused.
*
*       FEr#2018 - if the PCL action uses redirection to Router Lookup Translation Table (LTT)
*                 (policy based routing) or if the TTI action uses redirection to LTT
*                 (TTI base routing), then the LTT index written in the PCL/TTI action is
*                  restricted only to column 0 of the LTT row.
*       NOTE: - Since LTT entries can be used also for Router TCAM routing, this API is not
*               affected directly by this errata. However when the LTT should be pointed from
*               PCL action or LTT action, it should be set at column 0.
*             - The API affected by this errata are PCL rule set, PCL action update,
*               TTI rule set and TTI action update.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLttWrite
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          lttTtiRow,
    IN  GT_U32                          lttTtiColumn,
    IN  CPSS_DXCH_IP_LTT_ENTRY_STC      *lttEntryPtr
);


/*******************************************************************************
* cpssDxChIpLttRead
*
* DESCRIPTION:
*    Reads a LookUp Translation Table Entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - the device number.
*       lttTtiRow       - the entry's row index in LTT table (equivalent to
*                         the router tcam entry it is attached to)
*       lttTtiColumn    - the entry's column index in LTT table (equivalent to
*                         the router tcam entry it is attached to)
*
* OUTPUTS:
*       lttEntryPtr     - the lookup translation table entry
*
* RETURNS:
*       GT_OK           - if succeeded
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpLttWrite
*
*******************************************************************************/
GT_STATUS cpssDxChIpLttRead
(
    IN  GT_U8                              devNum,
    IN  GT_U32                             lttTtiRow,
    IN  GT_U32                             lttTtiColumn,
    OUT CPSS_DXCH_IP_LTT_ENTRY_STC         *lttEntryPtr
);


/*******************************************************************************
* cpssDxChIpUcRouteEntriesWrite
*
* DESCRIPTION:
*    Writes an array of uc route entries to hw.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum                - the device number
*       baseRouteEntryIndex   - the index from which to write the array
*       routeEntriesArray     - the uc route entries array
*       numOfRouteEntries     - the number route entries in the array (= the
*                               number of route entries to write)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In case uRPF ECMP/QOS is globally enabled, then for ECMP/QOS block
*       with x route entries, additional route entry should be included after
*       the block with the uRPF route entry information.
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpUcRouteEntriesWrite
(
    IN GT_U8                           devNum,
    IN GT_U32                          baseRouteEntryIndex,
    IN CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC *routeEntriesArray,
    IN GT_U32                          numOfRouteEntries
);


/*******************************************************************************
* cpssDxChIpUcRouteEntriesRead
*
* DESCRIPTION:
*    Reads an array of uc route entries from the hw.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum                - the device number
*       baseRouteEntryIndex   - the index from which to start reading
*       routeEntriesArray     - the uc route entries array, for each element
*                               set the uc route entry type to determine how
*                               entry data will be interpreted
*       numOfRouteEntries     - the number route entries in the array (= the
*                               number of route entries to read)
*
*
* OUTPUTS:
*       routeEntriesArray     - the uc route entries array read
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on devNum not active.
*       GT_BAD_PTR     - one of the parameters is NULL pointer
*       GT_BAD_STATE   - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpUcRouteEntriesWrite
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpUcRouteEntriesRead
(
    IN    GT_U8                             devNum,
    IN    GT_U32                            baseRouteEntryIndex,
    INOUT CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC   *routeEntriesArray,
    IN    GT_U32                            numOfRouteEntries
);



/*******************************************************************************
* cpssDxChIpMcRouteEntriesWrite
*
* DESCRIPTION:
*    Writes a MC route entry to hw.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum          - the device number
*       routeEntryIndex - the Index in the Route entries table
*       routeEntryPtr   - the MC route entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpMcRouteEntriesWrite
(
    IN GT_U8                           devNum,
    IN GT_U32                          routeEntryIndex,
    IN CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC *routeEntryPtr
);

/*******************************************************************************
* cpssDxChIpMcRouteEntriesRead
*
* DESCRIPTION:
*    Reads a MC route entry from the hw.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum          - the device number
*       routeEntryIndex - the route entry index.
*
* OUTPUTS:
*       routeEntryPtr   - the MC route entry read
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_BAD_STATE    - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpMcRouteEntriesRead
(
    IN GT_U8                            devNum,
    IN GT_U32                           routeEntryIndex,
    OUT CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC *routeEntryPtr
);

/*******************************************************************************
* cpssDxChIpRouterNextHopTableAgeBitsEntryWrite
*
* DESCRIPTION:
*    set router next hop table age bits entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum                              - the device number
*       routerNextHopTableAgeBitsEntryIndex - the router next hop table age bits
*                                             entry index. each entry is 32 Age
*                                             bits.
*       routerNextHopTableAgeBitsEntry      - a 32 Age Bit map for route entries
*                                             <32*routeEntriesIndexesArrayPtr>..
*                                             <32*routeEntriesIndexesArrayPtr+31>
*                                             bits.
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpRouterNextHopTableAgeBitsEntryWrite
(
    IN  GT_U8   devNum,
    IN  GT_U32  routerNextHopTableAgeBitsEntryIndex,
    IN  GT_U32  routerNextHopTableAgeBitsEntry
);

/*******************************************************************************
* cpssDxChIpRouterNextHopTableAgeBitsEntryRead
*
* DESCRIPTION:
*     read router next hop table age bits entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum                              - the device number
*       routerNextHopTableAgeBitsEntryIndex - the router next hop table age bits
*                                             entry index. each entry is 32 Age
*                                             bits.
* OUTPUTS:
*       routerNextHopTableAgeBitsEntryPtr   - a 32 Age Bit map for route entries
*                                             <32*routeEntriesIndexesArrayPtr>..
*                                             <32*routeEntriesIndexesArrayPtr+31>
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpRouterNextHopTableAgeBitsEntryRead
(
    IN  GT_U8   devNum,
    IN  GT_U32  routerNextHopTableAgeBitsEntryIndex,
    OUT GT_U32  *routerNextHopTableAgeBitsEntryPtr
);

/*******************************************************************************
* cpssDxChIpMLLPairWrite
*
* DESCRIPTION:
*    Write a Mc Link List (MLL) pair entry to hw.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum            - the device number
*       mllPairEntryIndex - the mll Pair entry index
*       mllPairWriteForm  - the way to write the Mll pair, first part only/
*                           second + next pointer only/ whole Mll pair
*       mllPairEntryPtr   - the Mc lisk list pair entry
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active or invalid mllPairWriteForm.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       MLL entries are two words long.
*       In Lion and above devices the whole entry is written when last word is
*       set.
*       In PP prior to lion The atomic hw write operation is done on one word.
*       Writing order of MLL parameters could be significant as some parameters
*       in word[1],[3] depend on parameters in word[0],[2].
*       Erroneous handling of mllRPFFailCommand/ nextHopTunnelPointer may
*       result with bad traffic. nextPointer may result with PP infinite loop.
*       cpssDxChIpMLLPairWrite handles the nextPointer parameter in the
*       following way:
*       if (last == 1) first set word[0] or word[2].
*       if (last == 0) first set word[3].
*
*******************************************************************************/
GT_STATUS cpssDxChIpMLLPairWrite
(
    IN GT_U8                                        devNum,
    IN GT_U32                                       mllPairEntryIndex,
    IN CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT           mllPairWriteForm,
    IN CPSS_DXCH_IP_MLL_PAIR_STC                    *mllPairEntryPtr
);

/*******************************************************************************
* cpssDxChIpMLLPairRead
*
* DESCRIPTION:
*    Read a Mc Link List (MLL) pair entry from the hw.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum            - the device number
*       mllPairEntryIndex - the mll Pair entry index
*       mllPairReadForm   - the way to read the Mll pair, first part only/
*                           second + next pointer only/ whole Mll pair
*
*
* OUTPUTS:
*       mllPairEntryPtr   - the Mc lisk list pair entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on invalid parameter.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpMLLPairRead
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      mllPairEntryIndex,
    IN CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT           mllPairReadForm,
    OUT CPSS_DXCH_IP_MLL_PAIR_STC                   *mllPairEntryPtr
);


/*******************************************************************************
* cpssDxChIpMLLLastBitWrite
*
* DESCRIPTION:
*    Write a Mc Link List (MLL) Last bit to the hw.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum            - the device number
*       mllPairEntryIndex - the mll Pair entry index
*       lastBit           - the Last bit
*       mllEntryPart      - to which part of the mll Pair to write the Last bit
*                           for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active or invalid mllEntryPart.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpMLLLastBitWrite
(
    IN GT_U8                                            devNum,
    IN GT_U32                                           mllPairEntryIndex,
    IN GT_BOOL                                          lastBit,
    IN CPSS_DXCH_IP_MLL_PAIR_PART_LAST_BIT_WRITE_ENT    mllEntryPart
);

/*******************************************************************************
* cpssDxChIpRouterArpAddrWrite
*
* DESCRIPTION:
*    write a ARP MAC address to the router ARP / Tunnel start / NAT table (for NAT capable devices)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - the device number
*       routerArpIndex  - The Arp Address index (to be inserted later
*                         in the UC Route entry Arp nextHopARPPointer
*                         field)
*       arpMacAddrPtr   - the ARP MAC address to write
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on devNum not active.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       pay attention that the router ARP table is shared with a tunnel start
*       table and NAT table (for NAT capable devices),
*       each tunnel start / NAT (for NAT capable devices) entry takes 4 ARP Address.
*       that is Tunnel start <n> / NAT (for NAT capable devices) <n>
*       takes ARP addresses <4n>,<4n+1>,<4n+2>,<4n+3>
*
*******************************************************************************/
GT_STATUS cpssDxChIpRouterArpAddrWrite
(
    IN GT_U8                         devNum,
    IN GT_U32                        routerArpIndex,
    IN GT_ETHERADDR                  *arpMacAddrPtr
);


/*******************************************************************************
* cpssDxChIpRouterArpAddrRead
*
* DESCRIPTION:
*    read a ARP MAC address from the router ARP / Tunnel start / NAT table (for NAT capable devices)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - the device number
*       routerArpIndex  - The Arp Address index (to be inserted later in the
*                         UC Route entry Arp nextHopARPPointer field)
*
*
* OUTPUTS:
*       arpMacAddrPtr   - the ARP MAC address
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on devNum not active.
*       GT_OUT_OF_RANGE          - on routerArpIndex bigger then 4095 or
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       see cpssDxChIpRouterArpAddrWrite
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpRouterArpAddrRead
(
    IN  GT_U8                        devNum,
    IN  GT_U32                       routerArpIndex,
    OUT GT_ETHERADDR                 *arpMacAddrPtr
);

/*******************************************************************************
* cpssDxChIpEcmpEntryWrite
*
* DESCRIPTION:
*       Write an ECMP entry
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; xCat2; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* INPUTS:
*       devNum          - the device number
*       ecmpEntryIndex  - the index of the entry in the ECMP table
*                         (APPLICABLE RANGES: 0..12287)
*       ecmpEntryPtr    - (pointer to) the ECMP entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChIpEcmpEntryWrite
(
    IN GT_U8                        devNum,
    IN GT_U32                       ecmpEntryIndex,
    IN CPSS_DXCH_IP_ECMP_ENTRY_STC  *ecmpEntryPtr
);

/*******************************************************************************
* cpssDxChIpEcmpEntryRead
*
* DESCRIPTION:
*       Read an ECMP entry
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; xCat2; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* INPUTS:
*       devNum          - the device number
*       ecmpEntryIndex  - the index of the entry in the ECMP table
*                         (APPLICABLE RANGES: 0..12287)
*
* OUTPUTS:
*       ecmpEntryPtr    - (pointer to) the ECMP entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChIpEcmpEntryRead
(
    IN  GT_U8                        devNum,
    IN  GT_U32                       ecmpEntryIndex,
    OUT CPSS_DXCH_IP_ECMP_ENTRY_STC  *ecmpEntryPtr
);

/*******************************************************************************
* cpssDxChIpEcmpHashNumBitsSet
*
* DESCRIPTION:
*       Set the start bit and the number of bits needed by the L3 ECMP hash
*       mechanism. Those parameters are used to select the ECMP member
*       within the ECMP block according to one of the following formulas:
*       - when randomEnable bit in the ECMP entry is GT_FALSE:
*           ECMP member offset =
*           ((size of ECMP block) * (the value received between [startBit] and
*           [startBit + numOfBits - 1])) >> numOfBits
*       - when randomEnable bit in the ECMP entry is GT_TRUE:
*           ECMP member offset =
*           ((size of ECMP block) * (random seed value)) >> numOfBits
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - the device number
*       startBit        - the index of the first bit that is needed by the L3
*                         ECMP hash mechanism (APPLICABLE RANGES: 0..31)
*       numOfBits       - the number of bits that is needed by the L3 ECMP hash
*                         mechanism (APPLICABLE RANGES: 1..16)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_OUT_OF_RANGE          - on wrong startBit or numOfBits
*
* COMMENTS:
*       startBit is relevant only when randomEnable field in the entry is set to
*       GT_FALSE.
*       startBit + numOfBits must not exceed 32.
*
*******************************************************************************/
GT_STATUS cpssDxChIpEcmpHashNumBitsSet
(
    IN GT_U8                        devNum,
    IN GT_U32                       startBit,
    IN GT_U32                       numOfBits
);

/*******************************************************************************
* cpssDxChIpEcmpHashNumBitsGet
*
* DESCRIPTION:
*       Get the start bit and the number of bits needed by the L3 ECMP hash
*       mechanism. Those parameters are used to select the ECMP member
*       within the ECMP block according to one of the following formulas:
*       - when randomEnable bit in the ECMP entry is GT_FALSE:
*           ECMP member offset =
*           ((size of ECMP block) * (the value received between [startBit] and
*           [startBit + numOfBits - 1])) >> numOfBits
*       - when randomEnable bit in the ECMP entry is GT_TRUE:
*           ECMP member offset =
*           ((size of ECMP block) * (random seed value)) >> numOfBits
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - the device number
*
* OUTPUTS:
*       startBitPtr     - (pointer to) the index of the first bit that is needed
*                         by the L3 ECMP hash mechanism
*       numOfBitsPtr    - (pointer to) the number of bits that is needed by the
*                         L3 ECMP hash mechanism
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       startBit is relevant only when randomEnable field in the entry is set to
*       GT_FALSE.
*
*******************************************************************************/
GT_STATUS cpssDxChIpEcmpHashNumBitsGet
(
    IN  GT_U8                       devNum,
    OUT GT_U32                      *startBitPtr,
    OUT GT_U32                      *numOfBitsPtr
);

/*******************************************************************************
* cpssDxChIpEcmpHashSeedValueSet
*
* DESCRIPTION:
*       Set the "seed value" parameter used by the L3 ECMP hash mechanism
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - the device number
*       seed            - the "seed value" parameter used by the L3 ECMP hash
*                         (APPLICABLE RANGES: 1..0xffffffff)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_OUT_OF_RANGE          - on wrong seed
*
* COMMENTS:
*       The "seed value" parameter is relevant only for ECMP entries that are
*       configured with randomEnable == GT_TRUE.
*       0 is illegal seed value.
*
*******************************************************************************/
GT_STATUS cpssDxChIpEcmpHashSeedValueSet
(
    IN GT_U8                        devNum,
    IN GT_U32                       seed
);

/*******************************************************************************
* cpssDxChIpEcmpHashSeedValueGet
*
* DESCRIPTION:
*       Get the "seed value" parameter used by the L3 ECMP hash mechanism
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - the device number
*
* OUTPUTS:
*       seedPtr         - (pointer to) the "seed value" parameter used by the L3
*                         ECMP hash
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       The "seed value" parameter is relevant only for ECMP entries that are
*       configured with randomEnable == GT_TRUE.
*
*******************************************************************************/
GT_STATUS cpssDxChIpEcmpHashSeedValueGet
(
    IN  GT_U8                       devNum,
    OUT GT_U32                      *seedPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChIph */

