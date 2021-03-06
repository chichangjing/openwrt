#include <Copyright.h>

/********************************************************************************
* msApiPrototype.h
*
* DESCRIPTION:
*       API Prototypes for QuarterDeck Device
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#ifndef __msApiPrototype_h
#define __msApiPrototype_h

#ifdef __cplusplus
extern "C" {
#endif
#include "msApiTypes.h"

/* gtBrgFdb.c */

/*******************************************************************************
* gfdbSetAtuSize
*
* DESCRIPTION:
*       Sets the Mac address table size.
*
* INPUTS:
*       size    - Mac address table size.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gfdbSetAtuSize
(
    IN GT_QD_DEV *dev,
    IN ATU_SIZE size
);


/*******************************************************************************
* gfdbGetAgingTimeRange
*
* DESCRIPTION:
*       Gets the maximal and minimum age times that the hardware can support.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       maxTimeout - max aging time in secounds.
*       minTimeout - min aging time in secounds.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on bad parameter
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gfdbGetAgingTimeRange
(
    IN GT_QD_DEV *dev,
    OUT GT_U32 *maxTimeout,
    OUT GT_U32 *minTimeout
);

/*******************************************************************************
* gfdbGetAgingTimeout
*
* DESCRIPTION:
*       Gets the timeout period in seconds for aging out dynamically learned
*       forwarding information. The returned value may not be the same as the value
*        programmed with <gfdbSetAgingTimeout>. Please refer to the description of
*        <gfdbSetAgingTimeout>.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       timeout - aging time in seconds.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gfdbGetAgingTimeout
(
    IN  GT_QD_DEV    *dev,
    OUT GT_U32       *timeout
);

/*******************************************************************************
* gfdbSetAgingTimeout
*
* DESCRIPTION:
*       Sets the timeout period in seconds for aging out dynamically learned
*       forwarding information. The standard recommends 300 sec.
*        Supported aging timeout values are multiple of time-base, where time-base
*        is either 15 or 16 seconds, depending on the Switch device. For example,
*        88E6063 uses time-base 16, and so supported aging timeouts are 0,16,32,
*        48,..., and 4080. If unsupported timeout value (bigger than 16) is used, 
*        the value will be rounded to the nearest supported value smaller than the 
*        given timeout. If the given timeout is less than 16, minimum timeout value
*        16 will be used instead. E.g.) 35 becomes 32 and 5 becomes 16.
*        <gfdbGetAgingTimeRange> function can be used to find the time-base.
*
* INPUTS:
*       timeout - aging time in seconds.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gfdbSetAgingTimeout
(
    IN GT_QD_DEV *dev,
    IN GT_U32 timeout
);



/*******************************************************************************
* gfdbGetAtuDynamicCount
*
* DESCRIPTION:
*       Gets the current number of dynamic unicast entries in this
*       Filtering Database.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       numDynEntries - number of dynamic entries.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NO_SUCH - vlan does not exist.
*
* COMMENTS:
*       None
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gfdbGetAtuDynamicCount
(
    IN GT_QD_DEV *dev,
    OUT GT_U32 *numDynEntries
);



/*******************************************************************************
* gfdbGetAtuEntryFirst
*
* DESCRIPTION:
*       Gets first lexicographic MAC address entry from the ATU.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       atuEntry - match Address translate unit entry.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NO_SUCH - table is empty.
*
* COMMENTS:
*       Search starts from Mac[00:00:00:00:00:00]
*
*        DBNum in atuEntry - 
*            ATU MAC Address Database number. If multiple address 
*            databases are not being used, DBNum should be zero.
*            If multiple address databases are being used, this value
*            should be set to the desired address database number.
*
*******************************************************************************/
GT_STATUS gfdbGetAtuEntryFirst
(
    IN GT_QD_DEV *dev,
    OUT GT_ATU_ENTRY    *atuEntry
);



/*******************************************************************************
* gfdbGetAtuEntryNext
*
* DESCRIPTION:
*       Gets next lexicographic MAC address from the specified Mac Addr.
*
* INPUTS:
*       atuEntry - the Mac Address to start the search.
*
* OUTPUTS:
*       atuEntry - match Address translate unit entry.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - on error or entry does not exist.
*       GT_NO_SUCH - no more entries.
*
* COMMENTS:
*       Search starts from atu.macAddr[xx:xx:xx:xx:xx:xx] specified by the
*       user.
*
*        DBNum in atuEntry - 
*            ATU MAC Address Database number. If multiple address 
*            databases are not being used, DBNum should be zero.
*            If multiple address databases are being used, this value
*            should be set to the desired address database number.
*
*******************************************************************************/
GT_STATUS gfdbGetAtuEntryNext
(
    IN GT_QD_DEV *dev,
    INOUT GT_ATU_ENTRY  *atuEntry
);



/*******************************************************************************
* gfdbFindAtuMacEntry
*
* DESCRIPTION:
*       Find FDB entry for specific MAC address from the ATU.
*
* INPUTS:
*       atuEntry - the Mac address to search.
*
* OUTPUTS:
*       found    - GT_TRUE, if the appropriate entry exists.
*       atuEntry - the entry parameters.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - on error or entry does not exist.
*       GT_NO_SUCH - no more entries.
*
* COMMENTS:
*        DBNum in atuEntry - 
*            ATU MAC Address Database number. If multiple address 
*            databases are not being used, DBNum should be zero.
*            If multiple address databases are being used, this value
*            should be set to the desired address database number.
*
*******************************************************************************/
GT_STATUS gfdbFindAtuMacEntry
(
    IN GT_QD_DEV *dev,
    INOUT GT_ATU_ENTRY  *atuEntry,
    OUT GT_BOOL         *found
);



/*******************************************************************************
* gfdbFlush
*
* DESCRIPTION:
*       This routine flush all or unblocked addresses from the MAC Address
*       Table.
*
* INPUTS:
*       flushCmd - the flush operation type.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_NO_RESOURCE  - failed to allocate a t2c struct
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gfdbFlush
(
    IN GT_QD_DEV *dev,
    IN GT_FLUSH_CMD flushCmd
);

/*******************************************************************************
* gfdbFlushInDB
*
* DESCRIPTION:
*       This routine flush all or unblocked addresses from the particular
*       ATU Database (DBNum). If multiple address databases are being used, this
*        API can be used to flush entries in a particular DBNum database.
*
* INPUTS:
*       flushCmd - the flush operation type.
*        DBNum     - ATU MAC Address Database Number. 
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_NOT_SUPPORTED- if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gfdbFlushInDB
(
    IN GT_QD_DEV *dev,
    IN GT_FLUSH_CMD flushCmd,
    IN GT_U32 DBNum
);

/*******************************************************************************
* gfdbAddMacEntry
*
* DESCRIPTION:
*       Creates the new entry in MAC address table.
*
* INPUTS:
*       macEntry    - mac address entry to insert to the ATU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_FAIL           - on error
*       GT_NO_RESOURCE    - failed to allocate a t2c struct
*       GT_OUT_OF_CPU_MEM - oaMalloc failed
*
* COMMENTS:
*        DBNum in atuEntry - 
*            ATU MAC Address Database number. If multiple address 
*            databases are not being used, DBNum should be zero.
*            If multiple address databases are being used, this value
*            should be set to the desired address database number.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gfdbAddMacEntry
(
    IN GT_QD_DEV *dev,
    IN GT_ATU_ENTRY *macEntry
);



/*******************************************************************************
* gfdbDelMacEntry
*
* DESCRIPTION:
*       Deletes MAC address entry.
*
* INPUTS:
*       macAddress - mac address.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_NO_RESOURCE  - failed to allocate a t2c struct
*       GT_NO_SUCH      - if specified address entry does not exist
*
* COMMENTS:
*       For SVL mode vlan Id is ignored.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gfdbDelMacEntry
(
    IN GT_QD_DEV *dev,
    IN GT_ETHERADDR  *macAddress
);

/*******************************************************************************
* gfdbDelAtuEntry
*
* DESCRIPTION:
*       Deletes ATU entry.
*
* INPUTS:
*       atuEntry - the ATU entry to be deleted.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_NO_RESOURCE  - failed to allocate a t2c struct
*       GT_NO_SUCH      - if specified address entry does not exist
*
* COMMENTS:
*        DBNum in atuEntry - 
*            ATU MAC Address Database number. If multiple address 
*            databases are not being used, DBNum should be zero.
*            If multiple address databases are being used, this value
*            should be set to the desired address database number.
*
*******************************************************************************/
GT_STATUS gfdbDelAtuEntry
(
    IN GT_QD_DEV *dev,
    IN GT_ATU_ENTRY  *atuEntry
);

/*******************************************************************************
* gfdbLearnEnable
*
* DESCRIPTION:
*       Enable/disable automatic learning of new source MAC addresses on port
*       ingress.
*
* INPUTS:
*       en - GT_TRUE for enable  or GT_FALSE otherwise
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gfdbLearnEnable
(
    IN GT_QD_DEV *dev,
    IN GT_BOOL  en
);


/*******************************************************************************
* gfdbGetLearnEnable
*
* DESCRIPTION:
*       Get automatic learning status of new source MAC addresses on port ingress.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       en - GT_TRUE if enabled  or GT_FALSE otherwise
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gfdbGetLearnEnable
(
    IN GT_QD_DEV    *dev,
    OUT GT_BOOL  *en
);

/*******************************************************************************
* gfdbGetLearnAVB
*
* DESCRIPTION:
*       Gets learn AVB bit. Learn on AVB frames. When this bit is cleared to a zero
*       AVB flows are not learned from and Source Address refreshed are not done on 
*       AVB frames nor are the Learn2All messages sent for these flow.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       learnAvb - learn AVB.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gfdbGetLearnAVB
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL        *learnAvb
);

/*******************************************************************************
* gfdbSetLearnAVB
*
* DESCRIPTION:
*       Sets learn AVB bit. Learn on AVB frames. When this bit is cleared to a zero
*       AVB flows are not learned from and Source Address refreshed are not done on 
*       AVB frames nor are the Learn2All messages sent for these flow.
*
* INPUTS:
*       learnAvb - learn AVB.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gfdbSetLearnAVB
(
    IN  GT_QD_DEV    *dev,
    IN  GT_BOOL      learnAvb
);

/*******************************************************************************
* gstpSetMode
*
* DESCRIPTION:
*       This routine Enable the Spanning tree.
*
* INPUTS:
*       en - GT_TRUE for enable, GT_FALSE for disable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       when enabled, this function sets all port to blocking state, and inserts
*       the BPDU MAC into the ATU to be captured to CPU, on disable all port are
*       being modified to be in forwarding state.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gstpSetMode
(
    IN GT_QD_DEV *dev,
    IN GT_BOOL  en
);



/*******************************************************************************
* gstpSetPortState
*
* DESCRIPTION:
*       This routine set the port state.
*
* INPUTS:
*       port  - the logical port number.
*       state - the port state to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gstpSetPortState
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT           port,
    IN GT_PORT_STP_STATE  state
);



/*******************************************************************************
* gstpGetPortState
*
* DESCRIPTION:
*       This routine returns the port state.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       state - the current port state.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gstpGetPortState
(
    IN GT_QD_DEV *dev,
    IN  GT_LPORT           port,
    OUT GT_PORT_STP_STATE  *state
);

/*******************************************************************************
* gprtSetEgressMode
*
* DESCRIPTION:
*       This routine set the egress mode.
*
* INPUTS:
*       port - the logical port number.
*       mode - the egress mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetEgressMode
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT        port,
    IN GT_EGRESS_MODE  mode
);



/*******************************************************************************
* gprtGetEgressMode
*
* DESCRIPTION:
*       This routine get the egress mode.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - the egress mode.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetEgressMode
(
    IN GT_QD_DEV *dev,
    IN  GT_LPORT        port,
    OUT GT_EGRESS_MODE  *mode
);



/*******************************************************************************
* gprtSetVlanTunnel
*
* DESCRIPTION:
*       This routine sets the vlan tunnel mode.
*
* INPUTS:
*       port - the logical port number.
*       mode - the vlan tunnel mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetVlanTunnel
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT port,
    IN GT_BOOL  mode
);



/*******************************************************************************
* gprtGetVlanTunnel
*
* DESCRIPTION:
*       This routine get the vlan tunnel mode.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - the vlan tunnel mode..
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_BAD_PARAM        - on bad parameters
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetVlanTunnel
(
    IN GT_QD_DEV *dev,
    IN  GT_LPORT port,
    OUT GT_BOOL  *mode
);


/*******************************************************************************
* gprtSetIGMPSnoop
*
* DESCRIPTION:
*         This routine set the IGMP Snoop. When set to one and this port receives
*        IGMP frame, the frame is switched to the CPU port, overriding all other 
*        switching decisions, with exception for CPU's Trailer.
*        CPU port is determined by the Ingress Mode bits. A port is considered 
*        the CPU port if its Ingress Mode are either GT_TRAILER_INGRESS or 
*        GT_CPUPORT_INGRESS.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE for IGMP Snoop or GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetIGMPSnoop
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT port,
    IN GT_BOOL  mode
);

/*******************************************************************************
* gprtGetIGMPSnoop
*
* DESCRIPTION:
*        This routine get the IGMP Snoop mode.
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE: IGMP Snoop enabled
*              GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetIGMPSnoop
(
    IN GT_QD_DEV *dev,
    IN  GT_LPORT port,
    OUT GT_BOOL  *mode
);

/* the following two APIs are added to support clippership */

/*******************************************************************************
* gprtSetHeaderMode
*
* DESCRIPTION:
*        This routine set ingress and egress header mode of a switch port. 
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE for header mode  or GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetHeaderMode
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT port,
    IN GT_BOOL  mode
);

/*******************************************************************************
* gprtGetHeaderMode
*
* DESCRIPTION:
*        This routine gets ingress and egress header mode of a switch port. 
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE: header mode enabled
*              GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetHeaderMode
(
    IN GT_QD_DEV *dev,
    IN  GT_LPORT port,
    OUT GT_BOOL  *mode
);


/*******************************************************************************
* gprtSetProtectedMode
*
* DESCRIPTION:
*        This routine set protected mode of a switch port. 
*        When this mode is set to GT_TRUE, frames are allowed to egress port
*        defined by the 802.1Q VLAN membership for the frame's VID 'AND'
*        by the port's VLANTable if 802.1Q is enabled on the port. Both must
*        allow the frame to Egress.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE for protected mode or GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetProtectedMode
(
    IN  GT_QD_DEV   *dev,
    IN GT_LPORT     port,
    IN GT_BOOL      mode
);

/*******************************************************************************
* gprtGetProtectedMode
*
* DESCRIPTION:
*        This routine gets protected mode of a switch port. 
*        When this mode is set to GT_TRUE, frames are allowed to egress port
*        defined by the 802.1Q VLAN membership for the frame's VID 'AND'
*        by the port's VLANTable if 802.1Q is enabled on the port. Both must
*        allow the frame to Egress.
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE: header mode enabled
*              GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetProtectedMode
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT        port,
    OUT GT_BOOL        *mode
);

/*******************************************************************************
* gprtSetForwardUnknown
*
* DESCRIPTION:
*        This routine set Forward Unknown mode of a switch port. 
*        When this mode is set to GT_TRUE, normal switch operation occurs.
*        When this mode is set to GT_FALSE, unicast frame with unknown DA addresses
*        will not egress out this port.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE for protected mode or GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetForwardUnknown
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT    port,
    IN GT_BOOL        mode
);

/*******************************************************************************
* gprtGetForwardUnknown
*
* DESCRIPTION:
*        This routine gets Forward Unknown mode of a switch port. 
*        When this mode is set to GT_TRUE, normal switch operation occurs.
*        When this mode is set to GT_FALSE, unicast frame with unknown DA addresses
*        will not egress out this port.
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE: header mode enabled
*                GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetForwardUnknown
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT        port,
    OUT GT_BOOL        *mode
);

/*******************************************************************************
* gprtGetSwitchReg
*
* DESCRIPTION:
*       This routine reads Switch Port Registers.
*
* INPUTS:
*       port    - logical port number
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetSwitchReg
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_U32         regAddr,
    OUT GT_U16         *data
);

/*******************************************************************************
* gprtSetSwitchReg
*
* DESCRIPTION:
*       This routine writes Switch Port Registers.
*
* INPUTS:
*       port    - logical port number
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetSwitchReg
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_U32         regAddr,
    IN  GT_U16         data
);


/*******************************************************************************
* gprtGetGlobalReg
*
* DESCRIPTION:
*       This routine reads Switch Global Registers.
*
* INPUTS:
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetGlobalReg
(
    IN  GT_QD_DEV    *dev,
    IN  GT_U32         regAddr,
    OUT GT_U16         *data
);

/*******************************************************************************
* gprtSetGlobalReg
*
* DESCRIPTION:
*       This routine writes Switch Global Registers.
*
* INPUTS:
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetGlobalReg
(
    IN  GT_QD_DEV    *dev,
    IN  GT_U32         regAddr,
    IN  GT_U16         data
);



/*******************************************************************************
* gvlnSetPortVlanPorts
*
* DESCRIPTION:
*       This routine sets the port VLAN group port membership list.
*
* INPUTS:
*       port        - logical port number to set.
*       memPorts    - array of logical ports.
*       memPortsLen - number of members in memPorts array
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_BAD_PARAM        - on bad parameters
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gvlnSetPortVlanPorts
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT port,
    IN GT_LPORT memPorts[],
    IN GT_U8    memPortsLen
);



/*******************************************************************************
* gvlnGetPortVlanPorts
*
* DESCRIPTION:
*       This routine gets the port VLAN group port membership list.
*
* INPUTS:
*       port        - logical port number to set.
*
* OUTPUTS:
*       memPorts    - array of logical ports.
*       memPortsLen - number of members in memPorts array
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_BAD_PARAM        - on bad parameters
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gvlnGetPortVlanPorts
(
    IN GT_QD_DEV *dev,
    IN  GT_LPORT port,
    OUT GT_LPORT memPorts[],
    OUT GT_U8    *memPortsLen
);




/*******************************************************************************
* gvlnSetPortUserPriLsb
*
* DESCRIPTION:
*       This routine Set the user priority (VPT) LSB bit, to be added to the
*       user priority on the egress.
*
* INPUTS:
*       port       - logical port number to set.
*       userPriLsb - GT_TRUE for 1, GT_FALSE for 0.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_BAD_PARAM        - on bad parameters
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gvlnSetPortUserPriLsb
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT port,
    IN GT_BOOL  userPriLsb
);



/*******************************************************************************
* gvlnGetPortUserPriLsb
*
* DESCRIPTION:
*       This routine gets the user priority (VPT) LSB bit.
*
* INPUTS:
*       port       - logical port number to set.
*
* OUTPUTS:
*       userPriLsb - GT_TRUE for 1, GT_FALSE for 0.
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_BAD_PARAM        - on bad parameters
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gvlnGetPortUserPriLsb
(
    IN GT_QD_DEV *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL     *userPriLsb
);


/*******************************************************************************
* gvlnSetPortVid
*
* DESCRIPTION:
*       This routine Set the port default vlan id.
*
* INPUTS:
*       port - logical port number to set.
*       vid  - the port vlan id.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_BAD_PARAM        - on bad parameters
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gvlnSetPortVid
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT port,
    IN GT_U16   vid
);


/*******************************************************************************
* gvlnGetPortVid
*
* DESCRIPTION:
*       This routine Get the port default vlan id.
*
* INPUTS:
*       port - logical port number to set.
*
* OUTPUTS:
*       vid  - the port vlan id.
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_BAD_PARAM        - on bad parameters
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gvlnGetPortVid
(
    IN GT_QD_DEV *dev,
    IN  GT_LPORT port,
    OUT GT_U16   *vid
);

/*******************************************************************************
* gvlnSetPortVlanDBNum
*
* DESCRIPTION:
*       This routine sets the port VLAN database number (DBNum).
*
* INPUTS:
*       port    - logical port number to set.
*       DBNum     - database number for this port 
*
* OUTPUTS:
*       None.
*
* RETURNS:IN GT_INGRESS_MODE mode
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_BAD_PARAM        - on bad parameters
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gvlnSetPortVlanDBNum
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT port,
    IN GT_U32   DBNum
);


/*******************************************************************************
* gvlnGetPortVlanDBNum
*
* DESCRIPTION:IN GT_INGRESS_MODE mode
*       This routine gets the port VLAN database number (DBNum).
*
* INPUTS:
*       port     - logical port number to get.
*
* OUTPUTS:
*       DBNum     - database number for this port 
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_BAD_PARAM        - on bad parameters
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gvlnGetPortVlanDBNum
(
    IN GT_QD_DEV *dev,
    IN  GT_LPORT port,
    OUT GT_U32    *DBNum
);

/********************************************************************
* gvlnSetPortVlanDot1qMode
*
* DESCRIPTION:
*       This routine sets the port 802.1q mode (11:10) 
*
* INPUTS:
*       port    - logical port number to set.
*       mode     - 802.1q mode for this port 
*
* OUTPUTS:
*       None.
*
* RETURNS:IN GT_INGRESS_MODE mode
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_BAD_PARAM        - on bad parameters
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gvlnSetPortVlanDot1qMode
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT     port,
    IN GT_DOT1Q_MODE    mode
);

/*******************************************************************************
* gvlnGetPortVlanDot1qMode
*
* DESCRIPTION:
*       This routine gets the port 802.1q mode (bit 11:10).
*
* INPUTS:
*       port     - logical port number to get.
*
* OUTPUTS:
*       mode     - 802.1q mode for this port 
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_BAD_PARAM        - on bad parameters
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gvlnGetPortVlanDot1qMode
(
    IN GT_QD_DEV *dev,
    IN  GT_LPORT port,
    OUT GT_DOT1Q_MODE    *mode
);


/********************************************************************
* gvlnSetPortVlanForceDefaultVID
*
* DESCRIPTION:
*       This routine sets the port 802.1q mode (11:10) 
*
* INPUTS:
*       port    - logical port number to set.
*       mode    - GT_TRUE, force to use default VID
*                 GT_FAULSE, otherwise 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_BAD_PARAM        - on bad parameters
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gvlnSetPortVlanForceDefaultVID
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT     port,
    IN GT_BOOL      mode
);

/*******************************************************************************
* gvlnGetPortVlanForceDefaultVID
*
* DESCRIPTION:
*       This routine gets the port mode for ForceDefaultVID (bit 12).
*
* INPUTS:
*       port     - logical port number to get.
*
* OUTPUTS:
*       mode     - ForceDefaultVID mode for this port 
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_BAD_PARAM        - on bad parameters
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gvlnGetPortVlanForceDefaultVID
(
    IN GT_QD_DEV *dev,
    IN  GT_LPORT      port,
    OUT GT_BOOL        *mode
);

/*******************************************************************************
* eventGetActive
*
* DESCRIPTION:
*       This routine gets enables/disables status of an hardware driven event.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       eventType - the event type. any combination of the folowing: 
*           GT_STATS_DONE, GT_VTU_PROB, GT_VTU_DONE, GT_ATU_FULL(or GT_ATU_PROB),
*           GT_ATU_DONE, GT_PHY_INTERRUPT, GT_EE_INTERRUPT, GT_DEVICE_INT,
*            and GT_AVB_INTERRUPT
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       Each switch device has its own set of event Types. Please refer to the
*        device datasheet for the list of event types that the device supports.
*
*******************************************************************************/
GT_STATUS eventGetActive
(
    IN  GT_QD_DEV  *dev,
    OUT GT_U32     *eventType
);

/*******************************************************************************
* eventSetActive
*
* DESCRIPTION:
*       This routine enables/disables the receive of an hardware driven event.
*
* INPUTS:
*       eventType - the event type. any combination of the folowing: 
*           GT_STATS_DONE, GT_VTU_PROB, GT_VTU_DONE, GT_ATU_FULL,  
*           GT_ATU_DONE, GT_PHY_INTERRUPT, GT_EE_INTERRUPT, and GT_DEVICE_INT
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       Each switch device has its own set of event Types. Please refer to the
*        device datasheet for the list of event types that the device supports.
*
*******************************************************************************/
GT_STATUS eventSetActive
(
    IN GT_QD_DEV     *dev,
    IN GT_U32         eventType
);

/*******************************************************************************
* eventGetIntStatus
*
* DESCRIPTION:
*       This routine reads an hardware driven event status.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       intCause -  It provides the source of interrupt of the following:
*                GT_STATS_DONE, GT_VTU_PROB, GT_VTU_DONE, GT_ATU_FULL,  
*                GT_ATU_DONE, GT_PHY_INTERRUPT, and GT_EE_INTERRUPT.
*                For Gigabit Switch, GT_ATU_FULL is replaced with GT_ATU_FULL and 
*                GT_PHY_INTERRUPT is not supported.
*                GT_DEVICE_INT may not be available on the devices, so please refer
*                to the datasheet for details.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS eventGetIntStatus
(
    IN  GT_QD_DEV     *dev,
    OUT GT_U16        *intCause
);

/*******************************************************************************
* gvtuGetIntStatus
*
* DESCRIPTION:
*         Check to see if a specific type of VTU interrupt occured
*
* INPUTS:
*       intType - the type of interrupt which causes an interrupt.
*            any combination of 
*            GT_MEMEBER_VIOLATION,
*            GT_MISS_VIOLATION,
*            GT_FULL_VIOLATION
*
* OUTPUTS:
*         None.
*
* RETURNS:
*         GT_OK   - on success
*         GT_FAIL - on error
*
* COMMENTS:
*         FULL_VIOLATION is only for Fast Ethernet Switch (not for Gigabit Switch).
*
*******************************************************************************/

GT_STATUS gvtuGetIntStatus
(
    IN  GT_QD_DEV             *dev,
    OUT GT_VTU_INT_STATUS     *vtuIntStatus
);

/*******************************************************************************
* gvtuGetEntryCount
*
* DESCRIPTION:
*       Gets the current number of entries in the VTU table
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       numEntries - number of VTU entries.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NO_SUCH - vlan does not exist.
*
* COMMENTS:
*       None
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gvtuGetEntryCount
(
    IN  GT_QD_DEV    *dev,
    OUT GT_U32         *numEntries
);

/*******************************************************************************
* gvtuGetEntryFirst
*
* DESCRIPTION:
*       Gets first lexicographic entry from the VTU.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       vtuEntry - match VTU entry.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NO_SUCH - table is empty.
*
* COMMENTS:
*       Search starts from vid of all one's
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gvtuGetEntryFirst
(
    IN  GT_QD_DEV         *dev,
    OUT GT_VTU_ENTRY    *vtuEntry
);

/*******************************************************************************
* gvtuGetEntryNext
*
* DESCRIPTION:
*       Gets next lexicographic VTU entry from the specified VID.
*
* INPUTS:
*       vtuEntry - the VID to start the search.
*
* OUTPUTS:
*       vtuEntry - match VTU  entry.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - on error or entry does not exist.
*       GT_NO_SUCH - no more entries.
*
* COMMENTS:
*       Search starts from the VID specified by the user.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gvtuGetEntryNext
(
    IN  GT_QD_DEV         *dev,
    INOUT GT_VTU_ENTRY  *vtuEntry
);

/*******************************************************************************
* gvtuFindVidEntry
*
* DESCRIPTION:
*       Find VTU entry for a specific VID, it will return the entry, if found, 
*       along with its associated data 
*
* INPUTS:
*       vtuEntry - contains the VID to search for.
*
* OUTPUTS:
*       found    - GT_TRUE, if the appropriate entry exists.
*       vtuEntry - the entry parameters.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - on error or entry does not exist.
*       GT_NO_SUCH - no more entries.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gvtuFindVidEntry
(
    IN GT_QD_DEV         *dev,
    INOUT GT_VTU_ENTRY  *vtuEntry,
    OUT GT_BOOL         *found
);

/*******************************************************************************
* gvtuFlush
*
* DESCRIPTION:
*       This routine removes all entries from VTU Table.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gvtuFlush
(
    IN GT_QD_DEV *dev
);

/*******************************************************************************
* gvtuAddEntry
*
* DESCRIPTION:
*       Creates the new entry in VTU table based on user input.
*
* INPUTS:
*       vtuEntry    - vtu entry to insert to the VTU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_FAIL           - on error
*       GT_FULL              - vtu table is full
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gvtuAddEntry
(
    IN GT_QD_DEV     *dev,
    IN GT_VTU_ENTRY *vtuEntry
);

/*******************************************************************************
* gvtuDelEntry
*
* DESCRIPTION:
*       Deletes VTU entry specified by user.
*
* INPUTS:
*       vtuEntry - the VTU entry to be deleted 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_NO_SUCH      - if specified address entry does not exist
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gvtuDelEntry
(
    IN GT_QD_DEV     *dev,
    IN GT_VTU_ENTRY *vtuEntry
);

/*******************************************************************************
* gvtuSetVTUControl
*
* DESCRIPTION:
*        Set VTU control bits. 
*
* INPUTS:
*        vtuCtrl - Combine Following VTU control bits
*                 GT_VTU_CONTROL_DONT_LEARN  
*                 GT_VTU_CONTROL_FILTER_UC   
*                 GT_VTU_CONTROL_FILTER_BC   
*                 GT_VTU_CONTROL_FILTER_MC    
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gvtuSetVTUControl
(
    IN GT_QD_DEV    *dev,
    IN GT_U16       vtuCtrl
);

/*******************************************************************************
* gvtuGetVTUControl
*
* DESCRIPTION:
*        Get VTU control bits. 
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        vtuCtrl - Combine Following VTU control bits
*                 GT_VTU_CONTROL_DONT_LEARN  
*                 GT_VTU_CONTROL_FILTER_UC   
*                 GT_VTU_CONTROL_FILTER_BC   
*                 GT_VTU_CONTROL_FILTER_MC    
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gvtuGetVTUControl
(
    IN  GT_QD_DEV    *dev,
    OUT GT_U16       *vtuCtrl
);

/*******************************************************************************
* gvtuSetVTUMode
*
* DESCRIPTION:
*        Set VTU Mode. The VTU Mode supports alternate VID look up results based 
*        on if a frame is a Multicast (including Broadcasts) or not, or based on 
*        what source port the frame came in on, or if the frame was properly 
*        Provider tagged or not.
* INPUTS:
*        mode - The mode of the VTU as follows:
*          00 = Use 4K VTU (VTU Page bit is zero unless overridden by the TCAM)
*          01 = Use 8K VTU w/VTU Page being a zero if a frame is a Unicast frame
*               (i.e., UC = 1)
*          10 = Use 8K VTU w/VTU Page being = to the source port�s VTU Page bit
*               (Port offset 0x05)
*          11 = Use 8K VTU w/VTU Page being a one if frame is not properly 
*               Provider tagged entering a Provider port (i.e., PTAG = 0)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gvtuSetVTUMode
(
    IN GT_QD_DEV    *dev,
    IN GT_U16       mode
);

/*******************************************************************************
* gvtuGetVTUMode
*
* DESCRIPTION:
*        Get VTU Mode. The VTU Mode supports alternate VID look up results based 
*        on if a frame is a Multicast (including Broadcasts) or not, or based on 
*        what source port the frame came in on, or if the frame was properly 
*        Provider tagged or not.
* INPUTS:
*        None.
*
* OUTPUTS:
*        mode - The mode of the VTU as follows:
*          00 = Use 4K VTU (VTU Page bit is zero unless overridden by the TCAM)
*          01 = Use 8K VTU w/VTU Page being a zero if a frame is a Unicast frame
*               (i.e., UC = 1)
*          10 = Use 8K VTU w/VTU Page being = to the source port�s VTU Page bit
*               (Port offset 0x05)
*          11 = Use 8K VTU w/VTU Page being a one if frame is not properly 
*               Provider tagged entering a Provider port (i.e., PTAG = 0)
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gvtuGetVTUMode
(
    IN GT_QD_DEV    *dev,
    OUT GT_U16      *mode
);

/*******************************************************************************
* gatuGetATUbin
*
* DESCRIPTION:
*        Get ATU Bin. ATU Get Next Bin Number. These bits contain the bin number 
*        where the last Get Next ATU operation found its entry. These bits are 
*        really part of the ATU previously found in register 0x0A.
* INPUTS:
*        None.
*
* OUTPUTS:
*        atuBin - ATU Bin:
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gatuGetATUbin
(
    IN GT_QD_DEV    *dev,
    OUT GT_U16      *atuBin
);

/*******************************************************************************
* gvtuSetVTUPage
*
* DESCRIPTION:
*        Set VTU Page. The VTU contains two Pages or two sets of the full 4K VID 
*        entries. When loading or reading an entry in the VTU, this bit determines
*        which page is being accessed. This bit is used along with the VID bits 
*        and can be considered as the 13th VID bit for all VTU Ops. 
*
* INPUTS:
*        page - The VTU Page: 0 or 1
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gvtuSetVTUPage
(
    IN GT_QD_DEV    *dev,
    IN GT_U16       page
);

/*******************************************************************************
* gvtuGetVTUPage
*
* DESCRIPTION:
*        Get VTU Page. The VTU contains two Pages or two sets of the full 4K VID 
*        entries. When loading or reading an entry in the VTU, this bit determines
*        which page is being accessed. This bit is used along with the VID bits 
*        and can be considered as the 13th VID bit for all VTU Ops. 
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        page - The VTU Page: 0 or 1
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gvtuGetVTUPage
(
    IN  GT_QD_DEV    *dev,
    OUT GT_U16       *page
);

/* gtPhyCtrl.c */

/*******************************************************************************
* gprtPhyReset
*
* DESCRIPTION:
*        This routine preforms PHY reset.
*        After reset, phy will be in Autonegotiation mode.
*
* INPUTS:
*         port - The logical port number
*
* OUTPUTS:
*         None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
* COMMENTS:
*         data sheet register 0.15 - Reset
*         data sheet register 0.13 - Speed
*         data sheet register 0.12 - Autonegotiation
*         data sheet register 0.8  - Duplex Mode
*
*******************************************************************************/

GT_STATUS gprtPhyReset
(
    IN GT_QD_DEV     *dev,
    IN GT_LPORT     port
);


/*******************************************************************************
* gprtSetPortLoopback
*
* DESCRIPTION:
* Enable/Disable Internal Port Loopback. 
* For 10/100 Fast Ethernet PHY, speed of Loopback is determined as follows:
*   If Auto-Negotiation is enabled, this routine disables Auto-Negotiation and 
*   forces speed to be 10Mbps.
*   If Auto-Negotiation is disabled, the forced speed is used.
*   Disabling Loopback simply clears bit 14 of control register(0.14). Therefore,
*   it is recommended to call gprtSetPortAutoMode for PHY configuration after 
*   Loopback test.
* For 10/100/1000 Gigagbit Ethernet PHY, speed of Loopback is determined as follows:
*   If Auto-Negotiation is enabled and Link is active, the current speed is used.
*   If Auto-Negotiation is disabled, the forced speed is used.
*   All other cases, default MAC Interface speed is used. Please refer to the data
*   sheet for the information of the default MAC Interface speed.
*
* INPUTS:
*         port - logical port number
*         enable - If GT_TRUE, enable loopback mode
*                     If GT_FALSE, disable loopback mode
*
* OUTPUTS:
*         None.
*
* RETURNS:
*         GT_OK     - on success
*         GT_FAIL     - on error
*
* COMMENTS:
*         data sheet register 0.14 - Loop_back
*
*******************************************************************************/

GT_STATUS gprtSetPortLoopback
(
    IN GT_QD_DEV     *dev,
    IN GT_LPORT     port,
    IN GT_BOOL         enable
);


/*******************************************************************************
* gprtSetPortSpeed
*
* DESCRIPTION:
*         Sets speed for a specific logical port. This function will keep the duplex 
*        mode and loopback mode to the previous value, but disable others, such as 
*        Autonegotiation.
*
* INPUTS:
*         port  - logical port number
*         speed - port speed.
*                PHY_SPEED_10_MBPS for 10Mbps
*                PHY_SPEED_100_MBPS for 100Mbps
*                PHY_SPEED_1000_MBPS for 1000Mbps
*
* OUTPUTS:
* None.
*
* RETURNS:
* GT_OK - on success
* GT_FAIL - on error
*
* COMMENTS:
* data sheet register 0.13 - Speed Selection (LSB)
* data sheet register 0.6  - Speed Selection (MSB)
*
*******************************************************************************/

GT_STATUS gprtSetPortSpeed
(
    IN GT_QD_DEV     *dev,
    IN GT_LPORT     port,
    IN GT_PHY_SPEED    speed
);


/*******************************************************************************
* gprtPortAutoNegEnable
*
* DESCRIPTION:
*         Enable/disable an Auto-Negotiation for duplex mode on specific
*         logical port. When Autonegotiation is disabled, phy will be in 10Mbps Half 
*        Duplex mode. Enabling Autonegotiation will set 100BASE-TX Full Duplex, 
*        100BASE-TX Full Duplex, 100BASE-TX Full Duplex, and 100BASE-TX Full Duplex
*        in AutoNegotiation Advertisement register.
*
* INPUTS:
*         port - logical port number
*         state - GT_TRUE for enable Auto-Negotiation for duplex mode,
*                     GT_FALSE otherwise
*
* OUTPUTS:
*         None.
*
* RETURNS:
*         GT_OK     - on success
*         GT_FAIL     - on error
*
* COMMENTS:
*         data sheet register 0.12 - Auto-Negotiation Enable
*         data sheet register 4.8, 4.7, 4.6, 4.5 - Auto-Negotiation Advertisement
*
*******************************************************************************/

GT_STATUS gprtPortAutoNegEnable
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL         state
);


/*******************************************************************************
* gprtPortPowerDown
*
* DESCRIPTION:
*         Enable/disable (power down) on specific logical port. When this function 
*        is called with normal operation request, phy will set to Autonegotiation 
*        mode.
*
* INPUTS:
*         port    - logical port number
*         state    -  GT_TRUE: power down
*                     GT_FALSE: normal operation
*
* OUTPUTS:
*         None.
*
* RETURNS:
*         GT_OK     - on success
*         GT_FAIL     - on error
*
* COMMENTS:
*         data sheet register 0.11 - Power Down
*
*******************************************************************************/

GT_STATUS gprtPortPowerDown
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port,
    IN GT_BOOL        state
);


/*******************************************************************************
* gprtPortRestartAutoNeg
*
* DESCRIPTION:
*         Restart AutoNegotiation. If AutoNegotiation is not enabled, it'll enable 
*        it. Loopback and Power Down will be disabled by this routine.
*
* INPUTS:
*         port - logical port number
*
* OUTPUTS:
*         None.
*
* RETURNS:
*         GT_OK     - on success
*         GT_FAIL     - on error
*
* COMMENTS:
*         data sheet register 0.9 - Restart Auto-Negotiation
*
*******************************************************************************/

GT_STATUS gprtPortRestartAutoNeg
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port
);


/*******************************************************************************
* gprtSetPortDuplexMode
*
* DESCRIPTION:
*         Sets duplex mode for a specific logical port. This function will keep 
*        the speed and loopback mode to the previous value, but disable others,
*        such as Autonegotiation.
*
* INPUTS:
*         port     - logical port number
*         dMode    - dulpex mode
*
* OUTPUTS:
*         None.
*
* RETURNS:
*         GT_OK     - on success
*         GT_FAIL     - on error
*
* COMMENTS:
*         data sheet register 0.8 - Duplex Mode
*
*******************************************************************************/

GT_STATUS gprtSetPortDuplexMode
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port,
    IN GT_BOOL        dMode
);


/*******************************************************************************
* gprtSetPortAutoMode
*
* DESCRIPTION:
*         This routine sets up the port with given Auto Mode.
*        Supported mode is as follows:
*        - Auto for both speed and duplex.
*        - Auto for speed only and Full duplex.
*        - Auto for speed only and Half duplex.
*        - Auto for duplex only and speed 1000Mbps.
*        - Auto for duplex only and speed 100Mbps.
*        - Auto for duplex only and speed 10Mbps.
*        - Speed 1000Mbps and Full duplex.
*        - Speed 1000Mbps and Half duplex.
*        - Speed 100Mbps and Full duplex.
*        - Speed 100Mbps and Half duplex.
*        - Speed 10Mbps and Full duplex.
*        - Speed 10Mbps and Half duplex.
*        
*
* INPUTS:
*         port - The logical port number
*         mode - Auto Mode to be written
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
* COMMENTS:
*         data sheet register 4.8, 4.7, 4.6, and 4.5 Autonegotiation Advertisement
*         data sheet register 4.6, 4.5 Autonegotiation Advertisement for 1000BX
*         data sheet register 9.9, 9.8 Autonegotiation Advertisement for 1000BT
*******************************************************************************/

GT_STATUS gprtSetPortAutoMode
(
    IN GT_QD_DEV     *dev,
    IN GT_LPORT     port,
    IN GT_PHY_AUTO_MODE mode
);

/*******************************************************************************
* gprtSetPause
*
* DESCRIPTION:
*       This routine will set the pause bit in Autonegotiation Advertisement
*        Register. And restart the autonegotiation.
*
* INPUTS:
* port - The logical port number
* state - GT_PHY_PAUSE_MODE enum value.
*            GT_PHY_NO_PAUSE        - disable pause
*             GT_PHY_PAUSE        - support pause
*            GT_PHY_ASYMMETRIC_PAUSE    - support asymmetric pause
*            GT_PHY_BOTH_PAUSE    - support both pause and asymmetric pause
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
* COMMENTS:
* data sheet register 4.10 Autonegotiation Advertisement Register
*******************************************************************************/

GT_STATUS gprtSetPause
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_PHY_PAUSE_MODE state
);

/*******************************************************************************
* gprtSetDTEDetect
*
* DESCRIPTION:
*       This routine enables/disables DTE.
*
* INPUTS:
*         port - The logical port number
*         mode - either GT_TRUE(for enable) or GT_FALSE(for disable)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/

GT_STATUS gprtSetDTEDetect
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT  port,
    IN GT_BOOL   state
);

/*******************************************************************************
* gprtGetDTEDetectStatus
*
* DESCRIPTION:
*       This routine gets DTE status.
*
* INPUTS:
*         port - The logical port number
*
* OUTPUTS:
*       status - GT_TRUE, if link partner needs DTE power.
*                 GT_FALSE, otherwise.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/

GT_STATUS gprtGetDTEDetectStatus
(
    IN  GT_QD_DEV *dev,
    IN  GT_LPORT  port,
    OUT GT_BOOL   *state
);

/*******************************************************************************
* gprtSetDTEDetectDropWait
*
* DESCRIPTION:
*       Once the PHY no longer detects that the link partner filter, the PHY
*        will wait a period of time before clearing the power over Ethernet 
*        detection status bit. The wait time is 5 seconds multiplied by the 
*        given value.
*
* INPUTS:
*         port - The logical port number
*       waitTime - 0 ~ 15 (unit of 4 sec.)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/

GT_STATUS gprtSetDTEDetectDropWait
(
    IN  GT_QD_DEV *dev,
    IN  GT_LPORT  port,
    IN  GT_U16    waitTime
);

/*******************************************************************************
* gprtGetDTEDetectDropWait
*
* DESCRIPTION:
*       Once the PHY no longer detects that the link partner filter, the PHY
*        will wait a period of time before clearing the power over Ethernet 
*        detection status bit. The wait time is 5 seconds multiplied by the 
*        returned value.
*
* INPUTS:
*         port - The logical port number
*
* OUTPUTS:
*       waitTime - 0 ~ 15 (unit of 4 sec.)
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/

GT_STATUS gprtGetDTEDetectDropWait
(
    IN  GT_QD_DEV *dev,
    IN  GT_LPORT  port,
    OUT GT_U16    *waitTime
);

/*******************************************************************************
* gprtSetEnergyDetect
*
* DESCRIPTION:
*       Energy Detect power down mode enables or disables the PHY to wake up on
*        its own by detecting activity on the CAT 5 cable. 
*
* INPUTS:
*         port - The logical port number
*       mode - GT_EDETECT_MODE type
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/

GT_STATUS gprtSetEnergyDetect
(
    IN  GT_QD_DEV *dev,
    IN  GT_LPORT  port,
    IN  GT_EDETECT_MODE   mode
);

/*******************************************************************************
* gprtGetEnergyDetect
*
* DESCRIPTION:
*       Energy Detect power down mode enables or disables the PHY to wake up on
*        its own by detecting activity on the CAT 5 cable. 
*
* INPUTS:
*         port - The logical port number
*
* OUTPUTS:
*       mode - GT_EDETECT_MODE type
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/

GT_STATUS gprtGetEnergyDetect
(
    IN  GT_QD_DEV *dev,
    IN  GT_LPORT  port,
    OUT GT_EDETECT_MODE   *mode
);

/*******************************************************************************
* gprtSet1000TMasterMode
*
* DESCRIPTION:
*       This routine sets the ports 1000Base-T Master mode and restart the Auto
*        negotiation.
*
* INPUTS:
*       port - the logical port number.
*       mode - GT_1000T_MASTER_SLAVE structure
*                autoConfig   - GT_TRUE for auto, GT_FALSE for manual setup.
*                masterPrefer - GT_TRUE if Master configuration is preferred.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSet1000TMasterMode
(
    IN  GT_QD_DEV   *dev,
    IN  GT_LPORT     port,
    IN  GT_1000T_MASTER_SLAVE   *mode
);

/*******************************************************************************
* gprtGet1000TMasterMode
*
* DESCRIPTION:
*       This routine retrieves 1000Base-T Master Mode
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - GT_1000T_MASTER_SLAVE structure
*                autoConfig   - GT_TRUE for auto, GT_FALSE for manual setup.
*                masterPrefer - GT_TRUE if Master configuration is preferred.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGet1000TMasterMode
(
    IN  GT_QD_DEV   *dev,
    IN  GT_LPORT     port,
    OUT GT_1000T_MASTER_SLAVE   *mode
);

/*******************************************************************************
* gprtGetPhyLinkStatus
*
* DESCRIPTION:
*       This routine retrieves the Link status.
*
* INPUTS:
*        port -    The logical port number, unless SERDES device is accessed
*                The physical address, if SERDES device is accessed
*
* OUTPUTS:
*       linkStatus - GT_FALSE if link is not established,
*                     GT_TRUE if link is established.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*        
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gprtGetPhyLinkStatus
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT  port,
    IN GT_BOOL      *linkStatus
);

/*******************************************************************************
* gprtSetPktGenEnable
*
* DESCRIPTION:
*       This routine enables or disables Packet Generator.
*       Link should be established first prior to enabling the packet generator,
*       and generator will generate packets at the speed of the established link.
*        When enables packet generator, the following information should be 
*       provided:
*           Payload Type:  either Random or 5AA55AA5
*           Packet Length: either 64 or 1514 bytes
*           Error Packet:  either Error packet or normal packet
*
* INPUTS:
*        port -    The logical port number, unless SERDES device is accessed
*                The physical address, if SERDES device is accessed
*       en      - GT_TRUE to enable, GT_FALSE to disable
*       pktInfo - packet information(GT_PG structure pointer), if en is GT_TRUE.
*                 ignored, if en is GT_FALSE
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*        
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gprtSetPktGenEnable
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT  port,
    IN GT_BOOL   en,
    IN GT_PG     *pktInfo
);


/*******************************************************************************
* gprtGetPhyReg
*
* DESCRIPTION:
*       This routine reads Phy Registers.
*
* INPUTS:
*       port -    The logical port number, 
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetPhyReg
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_U32         regAddr,
    OUT GT_U16         *data
);

/*******************************************************************************
* gprtSetPhyReg
*
* DESCRIPTION:
*       This routine writes Phy Registers.
*
* INPUTS:
*       port -    The logical port number, 
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetPhyReg
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT         port,
    IN  GT_U32           regAddr,
    IN  GT_U16           data
);

/*******************************************************************************
* gprtGetPagedPhyReg
*
* DESCRIPTION:
*       This routine reads phy register of the given page
*
* INPUTS:
*        port     - logical port to be read
*        regAddr    - register offset to be read
*        page    - page number to be read
*
* OUTPUTS:
*        data    - value of the read register
*
* RETURNS:
*       GT_OK               - if read successed
*       GT_FAIL               - if read failed
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gprtGetPagedPhyReg
(
    IN  GT_QD_DEV *dev,
    IN  GT_U32  port,
    IN    GT_U32  regAddr,
    IN    GT_U32  page,
    OUT GT_U16* data
);

/*******************************************************************************
* gprtSetPagedPhyReg
*
* DESCRIPTION:
*       This routine writes a value to phy register of the given page
*
* INPUTS:
*        port     - logical port to be read
*        regAddr    - register offset to be read
*        page    - page number to be read
*        data    - value of the read register
*
* OUTPUTS:
*        None
*
* RETURNS:
*       GT_OK               - if read successed
*       GT_FAIL               - if read failed
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gprtSetPagedPhyReg
(
    IN  GT_QD_DEV *dev,
    IN  GT_U32 port,
    IN    GT_U32 regAddr,
    IN    GT_U32 page,
    IN  GT_U16 data
);

/*******************************************************************************
* gprtGet45PhyReg
*
* DESCRIPTION:
*       This routine reads Phy Registers through Xmdio/Xmdc.
*
* INPUTS:
*       port        - The logical port number.
*       device      - The device address to read.
*       regAddr     - The register address to read.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGet45PhyReg
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_U32       device,
    IN  GT_U32       regAddr,
    OUT GT_U16         *data
);

/*******************************************************************************
* gprtSet45PhyReg
*
* DESCRIPTION:
*       This routine writes Phy Registers through Xmdio/Xmdc.
*
* INPUTS:
*       port        - The logical port number.
*       device      - The device address to read.
*       regAddr     - The register address to read.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSet45PhyReg
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT         port,
    IN  GT_U32           device,
    IN  GT_U32           regAddr,
    IN  GT_U16           data
);

/*******************************************************************************
* gprtPhyIntEnable
*
* DESCRIPTION:
* Enable/Disable one PHY Interrupt
* This register determines whether the INT# pin is asserted when an interrupt 
* event occurs. When an interrupt occurs, the corresponding bit is set and
* remains set until register 19 is read via the SMI. When interrupt enable
* bits are not set in register 18, interrupt status bits in register 19 are 
* still set when the corresponding interrupt events occur. However, the INT# 
* is not asserted.
*
* INPUTS:
* port    - logical port number
* intType - the type of interrupt to enable/disable. any combination of 
*            GT_SPEED_CHANGED,
*            GT_DUPLEX_CHANGED,
*            GT_PAGE_RECEIVED,
*            GT_AUTO_NEG_COMPLETED,
*            GT_LINK_STATUS_CHANGED,
*            GT_SYMBOL_ERROR,
*            GT_FALSE_CARRIER,
*            GT_FIFO_FLOW,
*            GT_CROSSOVER_CHANGED,
*            GT_POLARITY_CHANGED, and
*            GT_JABBER
*
* OUTPUTS:
* None.
*
* RETURNS:
* GT_OK - on success
* GT_FAIL - on error
*
* COMMENTS:
* 88E3081 data sheet register 18
*
*******************************************************************************/

GT_STATUS gprtPhyIntEnable
(
IN GT_QD_DEV *dev,
IN GT_LPORT   port,
IN GT_U16    intType
);


/*******************************************************************************
* gprtGetPhyIntStatus
*
* DESCRIPTION:
* Check to see if a specific type of  interrupt occured
*
* INPUTS:
* port - logical port number
* intType - the type of interrupt which causes an interrupt.
*            any combination of 
*            GT_SPEED_CHANGED,
*            GT_DUPLEX_CHANGED,
*            GT_PAGE_RECEIVED,
*            GT_AUTO_NEG_COMPLETED,
*            GT_LINK_STATUS_CHANGED,
*            GT_SYMBOL_ERROR,
*            GT_FALSE_CARRIER,
*            GT_FIFO_FLOW,
*            GT_CROSSOVER_CHANGED,
*            GT_POLARITY_CHANGED, and
*            GT_JABBER
*
* OUTPUTS:
* None.
*
* RETURNS:
* GT_OK - on success
* GT_FAIL - on error
*
* COMMENTS:
* 88E3081 data sheet register 19
*
*******************************************************************************/

GT_STATUS gprtGetPhyIntStatus
(
IN GT_QD_DEV *dev,
IN  GT_LPORT port,
OUT  GT_U16* intType
);

/*******************************************************************************
* gprtGetPhyIntPortSummary
*
* DESCRIPTION:
* Lists the ports that have active interrupts. It provides a quick way to 
* isolate the interrupt so that the MAC or switch does not have to poll the
* interrupt status register (19) for all ports. Reading this register does not
* de-assert the INT# pin
*
* INPUTS:
* none
*
* OUTPUTS:
* GT_U8 *intPortMask - bit Mask with the bits set for the corresponding 
* phys with active interrupt. E.g., the bit number 0 and 2 are set when 
* port number 0 and 2 have active interrupt
*
* RETURNS:
* GT_OK - on success
* GT_FAIL - on error
*
* COMMENTS:
* 88E3081 data sheet register 20
*
*******************************************************************************/

GT_STATUS gprtGetPhyIntPortSummary
(
IN GT_QD_DEV *dev,
OUT GT_U16 *intPortMask
);



/*******************************************************************************
* gprtSetForceFc
*
* DESCRIPTION:
*       This routine set the force flow control state.
*
* INPUTS:
*       port  - the logical port number.
*       force - GT_TRUE for force flow control  or GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetForceFc
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT port,
    IN GT_BOOL  force
);



/*******************************************************************************
* gprtGetForceFc
*
* DESCRIPTION:
*       This routine get the force flow control state.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       force - GT_TRUE for force flow control  or GT_FALSE otherwise
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetForceFc
(
    IN GT_QD_DEV *dev,
    IN  GT_LPORT port,
    OUT GT_BOOL  *force
);



/*******************************************************************************
* gprtSetTrailerMode
*
* DESCRIPTION:
*       This routine set the egress trailer mode.
*
* INPUTS:
*       port - the logical port number.
*       mode - GT_TRUE for add trailer or GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetTrailerMode
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT port,
    IN GT_BOOL  mode
);



/*******************************************************************************
* gprtGetTrailerMode
*
* DESCRIPTION:
*       This routine get the egress trailer mode.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - GT_TRUE for add trailer or GT_FALSE otherwise
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetTrailerMode
(
    IN GT_QD_DEV *dev,
    IN  GT_LPORT port,
    OUT GT_BOOL  *mode
);



/*******************************************************************************
* gprtSetIngressMode
*
* DESCRIPTION:
*       This routine set the ingress mode.
*
* INPUTS:
*       port - the logical port number.
*       mode - the ingress mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetIngressMode
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT        port,
    IN GT_INGRESS_MODE mode
);



/*******************************************************************************
* gprtGetIngressMode
*
* DESCRIPTION:
*       This routine get the ingress mode.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - the ingress mode.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetIngressMode
(
    IN GT_QD_DEV        *dev,
    IN  GT_LPORT        port,
    OUT GT_INGRESS_MODE *mode
);



/*******************************************************************************
* gprtSetMcRateLimit
*
* DESCRIPTION:
*       This routine set the port multicast rate limit.
*
* INPUTS:
*       port - the logical port number.
*       rate - GT_TRUE to Enable, GT_FALSE for otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetMcRateLimit
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT     port,
    IN GT_MC_RATE   rate
);



/*******************************************************************************
* gprtGetMcRateLimit
*
* DESCRIPTION:
*       This routine Get the port multicast rate limit.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       rate - GT_TRUE to Enable, GT_FALSE for otherwise.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetMcRateLimit
(
    IN GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_MC_RATE  *rate
);



/*******************************************************************************
* gprtSetCtrMode
*
* DESCRIPTION:
*       This routine sets the port counters mode of operation.
*
* INPUTS:
*       mode  - the counter mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetCtrMode
(
    IN GT_QD_DEV *dev,
    IN GT_CTR_MODE  mode
);



/*******************************************************************************
* gprtClearAllCtr
*
* DESCRIPTION:
*       This routine clears all port counters.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtClearAllCtr
(
    IN GT_QD_DEV *dev
);


/*******************************************************************************
* gprtGetPortCtr
*
* DESCRIPTION:
*       This routine gets the port counters.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       ctr - the counters value.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetPortCtr
(
    IN GT_QD_DEV *dev,
    IN  GT_LPORT        port,
    OUT GT_PORT_STAT    *ctr
);




/*******************************************************************************
* gprtGetPartnerLinkPause
*
* DESCRIPTION:
*       This routine retrives the link partner pause state.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       state - GT_TRUE for enable  or GT_FALSE otherwise
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetPartnerLinkPause
(
    IN GT_QD_DEV *dev,
    IN  GT_LPORT port,
    OUT GT_BOOL  *state
);



/*******************************************************************************
* gprtGetSelfLinkPause
*
* DESCRIPTION:
*       This routine retrives the link pause state.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       state - GT_TRUE for enable  or GT_FALSE otherwise
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetSelfLinkPause
(
    IN GT_QD_DEV *dev,
    IN  GT_LPORT port,
    OUT GT_BOOL  *state
);

/*******************************************************************************
* gprtGetTxPauseEn
*
* DESCRIPTION:
*        This routine retrives the link transmit pause state.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE for enable or GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gprtGetTxPauseEn
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL     *state
);

/*******************************************************************************
* gprtGetRxPauseEn
*
* DESCRIPTION:
*        This routine retrives the link receive pause state.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE for enable or GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gprtGetRxPauseEn
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL     *state
);

/*******************************************************************************
* gprtGetResolve
*
* DESCRIPTION:
*       This routine retrives the resolve state.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       state - GT_TRUE for Done  or GT_FALSE otherwise
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetResolve
(
    IN GT_QD_DEV *dev,
    IN  GT_LPORT port,
    OUT GT_BOOL  *state
);



/*******************************************************************************
* gprtGetLinkState
*
* DESCRIPTION:
*       This routine retrives the link state.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       state - GT_TRUE for Up  or GT_FALSE otherwise
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetLinkState
(
    IN GT_QD_DEV *dev,
    IN  GT_LPORT port,
    OUT GT_BOOL  *state
);



/*******************************************************************************
* gprtGetPortMode
*
* DESCRIPTION:
*       This routine retrives the port mode.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - GT_TRUE for MII  or GT_FALSE otherwise
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetPortMode
(
    IN GT_QD_DEV *dev,
    IN  GT_LPORT port,
    OUT GT_BOOL  *mode
);



/*******************************************************************************
* gprtGetPhyMode
*
* DESCRIPTION:
*       This routine retrives the PHY mode.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - GT_TRUE for MII PHY  or GT_FALSE otherwise
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetPhyMode
(
    IN GT_QD_DEV *dev,
    IN  GT_LPORT port,
    OUT GT_BOOL  *mode
);



/*******************************************************************************
* gprtGetDuplex
*
* DESCRIPTION:
*       This routine retrives the port duplex mode.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - GT_TRUE for Full  or GT_FALSE otherwise
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetDuplex
(
    IN GT_QD_DEV *dev,
    IN  GT_LPORT port,
    OUT GT_BOOL  *mode
);



/*******************************************************************************
* gprtGetSpeed
*
* DESCRIPTION:
*       This routine retrives the port speed.
*
* INPUTS:
*       speed - the logical port number.
*
* OUTPUTS:
*       mode - GT_TRUE for 100Mb/s  or GT_FALSE otherwise
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetSpeed
(
    IN GT_QD_DEV *dev,
    IN  GT_LPORT port,
    OUT GT_BOOL  *speed
);

/*******************************************************************************
* gprtSetDuplex
*
* DESCRIPTION:
*       This routine sets the duplex mode of MII/SNI/RMII ports.
*
* INPUTS:
*       port -     the logical port number.
*                (for FullSail, it will be port 2, and for ClipperShip, 
*                it could be either port 5 or port 6.)
*       mode -  GT_TRUE for Full Duplex,
*                GT_FALSE for Half Duplex.
*
* OUTPUTS: None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetDuplex
(
    IN GT_QD_DEV *dev,
    IN  GT_LPORT port,
    IN  GT_BOOL  mode
);


/*******************************************************************************
* gqosSetPortDefaultTc
*
* DESCRIPTION:
*       Sets the default traffic class for a specific port.
*
* INPUTS:
*       port      - logical port number
*       trafClass - default traffic class of a port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gcosSetPortDefaultTc
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT port,
    IN GT_U8    trafClass
);


/*******************************************************************************
* gcosGetPortDefaultTc
*
* DESCRIPTION:
*       Gets the default traffic class for a specific port.
*
* INPUTS:
*       port      - logical port number
*
* OUTPUTS:
*       trafClass - default traffic class of a port.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gcosGetPortDefaultTc
(
    IN  GT_QD_DEV *dev,
    IN GT_LPORT   port,
    OUT GT_U8     *trafClass
);


/*******************************************************************************
* gqosSetPrioMapRule
*
* DESCRIPTION:
*       This routine sets priority mapping rule.
*
* INPUTS:
*       port - the logical port number.
*       mode - GT_TRUE for user prio rule, GT_FALSE for otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gqosSetPrioMapRule
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT port,
    IN GT_BOOL  mode
);



/*******************************************************************************
* gqosGetPrioMapRule
*
* DESCRIPTION:
*       This routine get the priority mapping rule.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - GT_TRUE for user prio rule, GT_FALSE for otherwise.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gqosGetPrioMapRule
(
    IN GT_QD_DEV *dev,
    IN  GT_LPORT port,
    OUT GT_BOOL  *mode
);



/*******************************************************************************
* gqosIpPrioMapEn
*
* DESCRIPTION:
*       This routine enables the IP priority mapping.
*
* INPUTS:
*       port - the logical port number.
*       en   - GT_TRUE to Enable, GT_FALSE for otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gqosIpPrioMapEn
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT port,
    IN GT_BOOL  en
);



/*******************************************************************************
* gqosGetIpPrioMapEn
*
* DESCRIPTION:
*       This routine return the IP priority mapping state.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       en    - GT_TRUE for user prio rule, GT_FALSE for otherwise.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gqosGetIpPrioMapEn
(
    IN GT_QD_DEV *dev,
    IN  GT_LPORT port,
    OUT GT_BOOL  *en
);



/*******************************************************************************
* gqosUserPrioMapEn
*
* DESCRIPTION:
*       This routine enables the user priority mapping.
*
* INPUTS:
*       port - the logical port number.
*       en   - GT_TRUE to Enable, GT_FALSE for otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gqosUserPrioMapEn
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT port,
    IN GT_BOOL  en
);



/*******************************************************************************
* gqosGetUserPrioMapEn
*
* DESCRIPTION:
*       This routine return the user priority mapping state.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       en    - GT_TRUE for user prio rule, GT_FALSE for otherwise.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gqosGetUserPrioMapEn
(
    IN GT_QD_DEV *dev,
    IN  GT_LPORT port,
    OUT GT_BOOL  *en
);



/*******************************************************************************
* gcosGetUserPrio2Tc
*
* DESCRIPTION:
*       Gets the traffic class number for a specific 802.1p user priority.
*
* INPUTS:
*       userPrior - user priority
*
* OUTPUTS:
*       trClass - The Traffic Class the received frame is assigned.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*       Table - UserPrio2Tc
*
*******************************************************************************/
GT_STATUS gcosGetUserPrio2Tc
(
    IN GT_QD_DEV *dev,
    IN  GT_U8    userPrior,
    OUT GT_U8   *trClass
);


/*******************************************************************************
* gcosSetUserPrio2Tc
*
* DESCRIPTION:
*       Sets the traffic class number for a specific 802.1p user priority.
*
* INPUTS:
*       userPrior - user priority of a port.
*       trClass   - the Traffic Class the received frame is assigned.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*       Table - UserPrio2Tc
*
*******************************************************************************/
GT_STATUS gcosSetUserPrio2Tc
(
    IN GT_QD_DEV *dev,
    IN GT_U8    userPrior,
    IN GT_U8    trClass
);


/*******************************************************************************
* gcosGetDscp2Tc
*
* DESCRIPTION:
*       This routine retrieves the traffic class assigned for a specific
*       IPv4 Dscp.
*
* INPUTS:
*       dscp    - the IPv4 frame dscp to query.
*
* OUTPUTS:
*       trClass - The Traffic Class the received frame is assigned.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*       Table - UserPrio2Tc
*
*******************************************************************************/
GT_STATUS gcosGetDscp2Tc
(
    IN GT_QD_DEV *dev,
    IN  GT_U8   dscp,
    OUT GT_U8   *trClass
);


/*******************************************************************************
* gcosSetDscp2Tc
*
* DESCRIPTION:
*       This routine sets the traffic class assigned for a specific
*       IPv4 Dscp.
*
* INPUTS:
*       dscp    - the IPv4 frame dscp to map.
*       trClass - the Traffic Class the received frame is assigned.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*       Table - UserPrio2Tc
*
*******************************************************************************/
GT_STATUS gcosSetDscp2Tc
(
    IN GT_QD_DEV *dev,
    IN GT_U8    dscp,
    IN GT_U8    trClass
);

/* gtPortPrioMap.c  */
/*******************************************************************************
* gprtSetIpPrioMap
*
* DESCRIPTION:
*        This API set port IP Queue and Frame Priority Mapping.
*
* INPUTS:
*        port    - the logical port number
*        mapLoc  - Mapping table location.  DiffServ in recrived IP frame 
*        mapData - Mapping data.
*              ipYellow;   When the UseIpYellow bit is set to a one (Port offset 0x19) 
*				 this bit is used to indicate the color of frames. When the UseIpYellow 
*				 bit is cleared to a zero (Port offset 0x19) this bit is used 
*				 to indicate these frames are AVB stream frames.
*              disIpQpri;  
*              ipQpri;   
*              disIpFpri;   
*              ipFpri;   
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gprtSetIpPrioMap
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_U8       loc,
    IN  GT_PRIORITY_MAP_DATA    *mapdata
);

/*******************************************************************************
* gprtGetIpPrioMap
*
* DESCRIPTION:
*        This API get port IP Queue and Frame Priority Mapping.
*
* INPUTS:
*        port    - the logical port number
*        mapLoc  - Mapping table location.  DiffServ in recrived IP frame 
*
* OUTPUTS:
*        mapData - Mapping data.
*              ipYellow;   When the UseIpYellow bit is set to a one (Port offset 0x19) 
*				 this bit is used to indicate the color of frames. When the UseIpYellow 
*				 bit is cleared to a zero (Port offset 0x19) this bit is used 
*				 to indicate these frames are AVB stream frames.
*              disIpQpri;  
*              ipQpri;   
*              disIpFpri;   
*              ipFpri;   
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gprtGetIpPrioMap
(
    IN  GT_QD_DEV   *dev,
    IN  GT_LPORT    port,
    IN  GT_U8       loc,
    OUT GT_PRIORITY_MAP_DATA    *mapdata
);

/*******************************************************************************
* gprtSetIEEEPrioMap
*
* DESCRIPTION:
*        This API set port IEEE Priority Mapping.
*
* INPUTS:
*        port    - the logical port number
*        table    - Frame Priority Table set
*          GT_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI
*          GT_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP
*          GT_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP
*          GT_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP
*          GT_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP
*          GT_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP
*          GT_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP
*        mapLoc  - Mapping table location.  Pointer to the desired entry of 
*                  the IEEE Priority Mapping table selected above
*        mapData - Mapping data structure.
*              GT_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP_STRUCT;
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gprtSetIEEEPrioMap
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_IEEE_PRIORITY_TABLE_SET       table,
    IN  GT_U8       loc,
    IN  GT_IEEE_PRIORITY_TABLE_STRUCT    *prioTblStruct
);

/*******************************************************************************
* gprtGetIEEEPrioMap
*
* DESCRIPTION:
*        This API get port IEEE Priority Mapping.
*
* INPUTS:
*        port    - the logical port number
*        table    - Frame Priority Table set
*          GT_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI
*          GT_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP
*          GT_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP
*          GT_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP
*          GT_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP
*          GT_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP
*          GT_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP
*        mapLoc  - Mapping table location.  Pointer to the desired entry of 
*                  the IEEE Priority Mapping table selected above
*        mapData - Mapping data structure.
*              GT_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP_STRUCT;
*
* OUTPUTS:
*        mapData - Mapping data structure.
*              GT_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP_STRUCT;
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gprtGetIEEEPrioMap
(
    IN  GT_QD_DEV   *dev,
    IN  GT_LPORT    port,
    IN  GT_IEEE_PRIORITY_TABLE_SET       table,
    IN  GT_U8       loc,
    OUT GT_IEEE_PRIORITY_TABLE_STRUCT    *prioTblStruct
);

/*******************************************************************************
* qdLoadDriver
*
* DESCRIPTION:
*       QuarterDeck Driver Initialization Routine. 
*       This is the first routine that needs be called by system software. 
*       It takes sysCfg from system software, and retures a pointer (*dev) 
*       to a data structure which includes infomation related to this QuarterDeck
*       device. This pointer (*dev) is then used for all the API functions. 
*
* INPUTS:
*       sysCfg      - Holds system configuration parameters.
*
* OUTPUTS:
*       dev         - Holds general system information.
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_ALREADY_EXIST    - if device already started
*       GT_BAD_PARAM        - on bad parameters
*
* COMMENTS:
*     qdUnloadDriver is provided when the driver is not to be used anymore.
*
*******************************************************************************/
GT_STATUS qdLoadDriver
(
    IN  GT_SYS_CONFIG   *sysCfg,
    OUT GT_QD_DEV    *dev
);


/*******************************************************************************
* qdUnloadDriver
*
* DESCRIPTION:
*       This function unloads the QuaterDeck Driver.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       1.  This function should be called only after successful execution of
*           qdLoadDriver().
*
*******************************************************************************/
GT_STATUS qdUnloadDriver
(
    IN GT_QD_DEV* dev
);


/*******************************************************************************
* sysEnable
*
* DESCRIPTION:
*       This function enables the system for full operation.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS sysEnable
(
    IN GT_QD_DEV* dev
);


/*******************************************************************************
* gsysSwReset
*
* DESCRIPTION:
*       This routine preforms switch software reset.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysSwReset
(
    IN GT_QD_DEV* dev
);


/*******************************************************************************
* gsysSetDiscardExcessive
*
* DESCRIPTION:
*       This routine set the Discard Excessive state.
*
* INPUTS:
*       en - GT_TRUE Discard is enabled, GT_FALSE otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysSetDiscardExcessive
(
    IN GT_QD_DEV* dev,
    IN GT_BOOL en
);



/*******************************************************************************
* gsysGetDiscardExcessive
*
* DESCRIPTION:
*       This routine get the Discard Excessive state.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       en - GT_TRUE Discard is enabled, GT_FALSE otherwise.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on bad parameter
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetDiscardExcessive
(
    IN GT_QD_DEV* dev,
    IN GT_BOOL *en
);



/*******************************************************************************
* gsysSetSchedulingMode
*
* DESCRIPTION:
*       This routine set the Scheduling Mode(old).
*
* INPUTS:
*       mode - GT_TRUE wrr, GT_FALSE strict.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysSetSchedulingMode
(
    IN GT_QD_DEV* dev,
    IN GT_BOOL mode
);



/*******************************************************************************
* gsysGetSchedulingMode
*
* DESCRIPTION:
*       This routine get the Scheduling Mode(old).
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       mode - GT_TRUE wrr, GT_FALSE strict.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on bad parameter
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetSchedulingMode
(
    IN GT_QD_DEV *dev,
    OUT GT_BOOL *mode
);

/*******************************************************************************
* gsysSetHashMulticast
*
* DESCRIPTION:
*       This routine set the Hash Multicast.
*       When this bit is cleared to a zero multicast MAC address
*       lookups in the ATU are done using the Direct HashSel setting reqardless
*       of the current setting of the HashSel bits. All unicast MAC address
*       lookups use the current HashSel setting. When this bit is set to a one
*       all MAC address lookups in the ATU are done using the current HashSel
*       setting.
*
* INPUTS:
*       mode - GT_TRUE or GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysSetHashMulticast
(
    IN  GT_QD_DEV *dev,
    IN GT_BOOL    mode
);

/*******************************************************************************
* gsysGetHashMulticast
*
* DESCRIPTION:
*       This routine set the Hash Multicast.
*       When this bit is cleared to a zero multicast MAC address
*       lookups in the ATU are done using the Direct HashSel setting reqardless
*       of the current setting of the HashSel bits. All unicast MAC address
*       lookups use the current HashSel setting. When this bit is set to a one
*       all MAC address lookups in the ATU are done using the current HashSel
*       setting.
*
* INPUTS:
*       mode - GT_TRUE or GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on bad parameter
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetHashMulticast
(
    IN  GT_QD_DEV *dev,
    OUT GT_BOOL   *mode
);

/*******************************************************************************
* gsysSetMaxFrameSize
*
* DESCRIPTION:
*       This routine Set the max frame size allowed.
*
* INPUTS:
*       mode - GT_TRUE max size 1522, GT_FALSE max size 1535.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysSetMaxFrameSize
(
    IN GT_QD_DEV* dev,
    IN GT_BOOL mode
);



/*******************************************************************************
* gsysGetMaxFrameSize
*
* DESCRIPTION:
*       This routine Get the max frame size allowed.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       mode - GT_TRUE max size 1522, GT_FALSE max size 1535.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on bad parameter
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetMaxFrameSize
(
    IN GT_QD_DEV *dev,
    OUT GT_BOOL *mode
);



/*******************************************************************************
* gsysReLoad
*
* DESCRIPTION:
*       This routine cause to the switch to reload the EEPROM.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysReLoad
(
    IN GT_QD_DEV* dev
);


/*******************************************************************************
* gsysSetWatchDog
*
* DESCRIPTION:
*       This routine Set the the watch dog mode.
*
* INPUTS:
*       en - GT_TRUE enables, GT_FALSE disable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysSetWatchDog
(
    IN GT_QD_DEV* dev,
    IN GT_BOOL en
);



/*******************************************************************************
* gsysGetWatchDog
*
* DESCRIPTION:
*       This routine Get the the watch dog mode.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       en - GT_TRUE enables, GT_FALSE disable.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on bad parameter
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetWatchDog
(
    IN GT_QD_DEV* dev,
    OUT GT_BOOL *en
);


/*******************************************************************************
* gsysSetDuplexPauseMac
*
* DESCRIPTION:
*       This routine sets the full duplex pause src Mac Address.
*
* INPUTS:
*       mac - The Mac address to be set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on bad parameter
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysSetDuplexPauseMac
(
    IN GT_QD_DEV* dev,
    IN GT_ETHERADDR *mac
);


/*******************************************************************************
* gsysGetDuplexPauseMac
*
* DESCRIPTION:
*       This routine Gets the full duplex pause src Mac Address.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       mac - the Mac address.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on bad parameter
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetDuplexPauseMac
(
    IN GT_QD_DEV* dev,
    OUT GT_ETHERADDR *mac
);



/*******************************************************************************
* gsysSetPerPortDuplexPauseMac
*
* DESCRIPTION:
*       This routine sets whether the full duplex pause src Mac Address is per
*       port or per device.
*
* INPUTS:
*       en - GT_TURE per port mac, GT_FALSE global mac.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysSetPerPortDuplexPauseMac
(
    IN GT_QD_DEV* dev,
    IN GT_BOOL en
);



/*******************************************************************************
* gsysGetPerPortDuplexPauseMac
*
* DESCRIPTION:
*       This routine Gets whether the full duplex pause src Mac Address is per
*       port or per device.
*
* INPUTS:
*       en - GT_TURE per port mac, GT_FALSE global mac.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on bad parameter
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetPerPortDuplexPauseMac
(
    IN GT_QD_DEV* dev,
    IN GT_BOOL *en
);

/*******************************************************************************
* gsysSetPortWakeonFrameEn
*
* DESCRIPTION:
*       This routine sets port interrupt for wake on frame.
*
* INPUTS:
*       portVec - combine port interrupt enable=1 disable=0:
*                 port 0: bit0, port 1: bit1, port 2: bit2, ...
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysSetPortWakeonFrameEn
(
    IN GT_QD_DEV    *dev,
    IN GT_U8        portVec
);
GT_STATUS gsysSetPortWakeonFrameEn_11p
(
    IN GT_QD_DEV    *dev,
    IN GT_U32        portVec
);

/*******************************************************************************
* gsysGetPortWakeonFrameEnSt
*
* DESCRIPTION:
*       This routine gets port interrupt status for wake on frame.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       portVec - combine port interrupt enable=1 disable=0:
*                 port 0: bit0, port 1: bit1, port 2: bit2, ...
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetPortWakeonFrameEnSt
(
    IN GT_QD_DEV    *dev,
    OUT GT_U8       *portVec
);
GT_STATUS gsysGetPortWakeonFrameEnSt_11p
(
    IN GT_QD_DEV    *dev,
    OUT GT_U32       *portVec
);

/*******************************************************************************
* gsysGetPortWakeonFrameEn
*
* DESCRIPTION:
*       This routine gets port interrupt enable for wake on frame.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       portVec - combine port interrupt enable=1 disable=0:
*                 port 0: bit0, port 1: bit1, port 2: bit2, ...
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetPortWakeonFrameEn
(
    IN GT_QD_DEV    *dev,
    OUT GT_U8       *portVec
);
GT_STATUS gsysGetPortWakeonFrameEn_11p
(
    IN GT_QD_DEV    *dev,
    OUT GT_U32       *portVec
);

/*******************************************************************************
* gsysSetWoLMac
*
* DESCRIPTION:
*       This routine sets the Wake on Lan Mac Address.
*        MAC address should be an Unicast address.
*        For different MAC Addresses per port operation, 
*        use gsysSetPerPortDuplexPauseMac API.
*
* INPUTS:
*       mac - The Mac address to be set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on bad parameter
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gsysSetWoLMac
(
    IN GT_QD_DEV    *dev,
    IN GT_ETHERADDR *mac
);

/*******************************************************************************
* gsysGetWoLMac
*
* DESCRIPTION:
*       This routine Gets the Wake on Lan Mac Address.
*        For different MAC Addresses per port operation, 
*        use gsysGetPerPortDuplexPauseMac API.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       mac - the Mac address.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on bad parameter
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gsysGetWoLMac
(
    IN  GT_QD_DEV    *dev,
    OUT GT_ETHERADDR *mac
);

/*******************************************************************************
* gsysSetPerPortWoLMac
*
* DESCRIPTION:
*       This routine sets whether the Wake on Lan Mac Address is per
*       port or per device.
*
* INPUTS:
*       en - GT_TURE per port mac, GT_FALSE global mac.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysSetPerPortWoLMac
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL      en
);

/*******************************************************************************
* gsysGetPerPortWoLMac
*
* DESCRIPTION:
*       This routine Gets whether the Wake on Lanc Mac Address is per
*       port or per device.
*
* INPUTS:
*       en - GT_TURE per port mac, GT_FALSE global mac.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on bad parameter
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetPerPortWoLMac
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL      *en
);

/*******************************************************************************
* gsysSetWoLPass
*
* DESCRIPTION:
*       This routine sets the Wake on Lan Password Mac Address.
*
* INPUTS:
*       mac - The Mac address to be set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on bad parameter
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gsysSetWoLPass
(
    IN GT_QD_DEV    *dev,
    IN GT_ETHERADDR *mac
);

/*******************************************************************************
* gsysGetWoLPass
*
* DESCRIPTION:
*       This routine Gets the Wake on Lan password Mac Address.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       mac - the Mac address.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on bad parameter
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gsysGetWoLPass
(
    IN  GT_QD_DEV    *dev,
    OUT GT_ETHERADDR *mac
);

/*******************************************************************************
* gsysSetWolControl
*
* DESCRIPTION:
*       This routine sets Wake on LAN Mode on port.
*
* INPUTS:
*       port - port number
*       mode - The supported WoL Modes are:
*          GT_WOL_CTRL_MODE_DISABLE        WoL disabled 
*          GT_WOL_CTRL_MODE_EN_CHK_8       WoL enabled checking for 8 instances of the WoL Address 
*          GT_WOL_CTRL_MODE_EN_CHK_16      WoL enabled checking for 16 instances of the WoL Address 
*          GT_WOL_CTRL_MODE_EN_CHK_16_PASS WoL enabled checking for 16 instances of the WoL Address followed by the WoL Password
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysSetWolControl
(
    IN GT_QD_DEV    *dev,
    IN GT_U8        port,
    IN GT_U8        mode
);

/*******************************************************************************
* gsysGetWolControl
*
* DESCRIPTION:
*       This routine gets Wake on LAN Mode on port.
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*       mode - The supported WoL Modes are:
*          GT_WOL_CTRL_MODE_DISABLE        WoL disabled 
*          GT_WOL_CTRL_MODE_EN_CHK_8       WoL enabled checking for 8 instances of the WoL Address 
*          GT_WOL_CTRL_MODE_EN_CHK_16      WoL enabled checking for 16 instances of the WoL Address 
*          GT_WOL_CTRL_MODE_EN_CHK_16_PASS WoL enabled checking for 16 instances of the WoL Address followed by the WoL Password
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetWolControl
(
    IN GT_QD_DEV    *dev,
    IN GT_U8        port,
    OUT GT_U8       *mode
);

/*******************************************************************************
* gsysSetWolControlMc
*
* DESCRIPTION:
*       This routine sets Multicast address for Wake on LAN Mode on port.
*
* INPUTS:
*       mc - GT_TRUE or GT_HALSE
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysSetWolControlMc
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL      mc
);

/*******************************************************************************
* gsysGetWolControlMc
*
* DESCRIPTION:
*       This routine gets Multicast address for Wake on LAN Mode on port.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       mc - GT_TRUE or GT_HALSE
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetWolControlMc
(
    IN GT_QD_DEV    *dev,
    OUT GT_BOOL     *mc
);

/*******************************************************************************
* gsysGetWakeupSource
*
* DESCRIPTION:
*       This routine gets Wakeup interrupt source.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       wsrc - wake up source.
*         GT_WAKE_INT_SOURCE_WOF     Wake on Frame Interrupt
*         GT_WAKE_INT_SOURCE_WOL_0   Wake on LAN Port 0 
*         GT_WAKE_INT_SOURCE_WOL_1   Wake on LAN Port 1 
*         GT_WAKE_INT_SOURCE_WOL_2   Wake on LAN Port 2 
*         GT_WAKE_INT_SOURCE_WOL_3   Wake on LAN Port 3 
*         GT_WAKE_INT_SOURCE_WOL_4   Wake on LAN Port 4 
*         GT_WAKE_INT_SOURCE_WOL_5   Wake on LAN Port 5 
*         GT_WAKE_INT_SOURCE_WOL_6   Wake on LAN Port 6 
*         GT_WAKE_INT_SOURCE_WOL_7   Wake on LAN Port 7 
*         GT_WAKE_INT_SOURCE_WOL_8   Wake on LAN Port 8 
*         GT_WAKE_INT_SOURCE_WOL_9   Wake on LAN Port 9 
*         GT_WAKE_INT_SOURCE_WOL_10  Wake on LAN Port 10 
*         GT_WAKE_INT_SOURCE_WOL_11  Wake on LAN Port 11 
*         GT_WAKE_INT_SOURCE_WOL_NO  Wake on LAN Port, 0xf: no port, other number=port 
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetWakeupSource
(
    IN GT_QD_DEV    *dev,
    OUT GT_U8       *wsrc
);

/*******************************************************************************
* gsysClearWakeupSource
*
* DESCRIPTION:
*       This routine clear Wakeup interrupt source.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysClearWakeupSource
(
    IN GT_QD_DEV    *dev
);

/*******************************************************************************
* gsysSetGenMac
*
* DESCRIPTION:
*       This routine sets Generic MAC Address 1/2/3 MAC 1, MAC 2 and MAC 3,
*       (used by Egress TCAM Actions).
*
* INPUTS:
*       macNum - 1: Mac 1, 2: Mac 2, 3: Mac 3.
*       mac - The Mac address to be set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on bad parameter
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gsysSetGenMac
(
    IN GT_QD_DEV    *dev,
	IN GT_32        macNum,
    IN GT_ETHERADDR *mac
);

/*******************************************************************************
* gsysGetGenMac
*
* DESCRIPTION:
*       This routine gets Generic MAC Address 1/2/3 MAC 1, MAC 2 and MAC 3,
*       (used by Egress TCAM Actions).
*
* INPUTS:
*       macNum - 1: Mac 1, 2: Mac 2, 3: Mac 3.
*
* OUTPUTS:
*       mac - the Mac address.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on bad parameter
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gsysGetGenMac
(
    IN  GT_QD_DEV    *dev,
	IN GT_32        macNum,
    OUT GT_ETHERADDR *mac
);

/*******************************************************************************
* gsysSetCutThroughHold
*
* DESCRIPTION:
*    Set Cut Through Burst Hold amount. To support bursts of frames in Cut Through
*    mode, once a Cut Through connection is made between ports, the Cut
*    Through connection needs to be held beyond the end of each transmitted
*    frame. This hold time keeps the last Cut Through connection active until the
*    next ingressing frame can be processed to see if it is also to be Cut Through.
*
* INPUTS:
*        number - the number of octets a Cut Through connection is held after 
*          the last bytes of a frame�s CRC is transmitted.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM    - on bad parameter
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetCutThroughHold
(
    IN GT_QD_DEV    *dev,
    IN GT_U8        number
);

/*******************************************************************************
* gsysGetCutThroughHold
*
* DESCRIPTION:
*    Get Cut Through Burst Hold amount. To support bursts of frames in Cut Through
*    mode, once a Cut Through connection is made between ports, the Cut
*    Through connection needs to be held beyond the end of each transmitted
*    frame. This hold time keeps the last Cut Through connection active until the
*    next ingressing frame can be processed to see if it is also to be Cut Through.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        number - the number of octets a Cut Through connection is held after 
*          the last bytes of a frame�s CRC is transmitted.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM    - on bad parameter
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysGetCutThroughHold
(
    IN GT_QD_DEV    *dev,
    OUT GT_U8       *number
);

/*******************************************************************************
* gsysSetCTTooEarly
*
* DESCRIPTION:
*    Set Cut Through Point Too Early. This bit is set to a one whenever a Cut
*    Through is attempted where the results of the ATU were not known due
*    to the ATU being busy (see the Cut Through Point register below). This
*    bit will return to zero only by software writing a zero to this bit. <<Most
*    interrupt bits are ROC so that new interrupts are not missed while one is
*    being cleared. Missing one of these interrupts right after clearing one is
*    not an issue to the intended operation in this case.>>
*
* INPUTS:
*        earlyBit - Cut Through Point Too Early bit.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM    - on bad parameter
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetCTTooEarly
(
    IN GT_QD_DEV    *dev,
    IN GT_U8        earlyBit
);

/*******************************************************************************
* gsysGetCTTooEarly
*
* DESCRIPTION:
*    Get Cut Through Point Too Early. This bit is set to a one whenever a Cut
*    Through is attempted where the results of the ATU were not known due
*    to the ATU being busy (see the Cut Through Point register below). This
*    bit will return to zero only by software writing a zero to this bit. <<Most
*    interrupt bits are ROC so that new interrupts are not missed while one is
*    being cleared. Missing one of these interrupts right after clearing one is
*    not an issue to the intended operation in this case.>>
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        earlyBit - Cut Through Point Too Early bit.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM    - on bad parameter
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysGetCTTooEarly
(
    IN GT_QD_DEV    *dev,
    OUT GT_U8       *earlyBit
);

/*******************************************************************************
* gsysSetCTTooEarlyIntEn
*
* DESCRIPTION:
*    Set Cut Through Point Too Early Interrupt Enable. When this bit is set to a
*    one the CTTooEarly bit (above) being a one will generate a Watch Dog
*    interrupt (Global 2 offset 0x00 and Global 2 offset 0x1B). When this bit is
*    cleared to a zero, the CTTooEarly bit being a one will not generate a
*    Watch Dog interrupt.
*
* INPUTS:
*        en - GT_TRUE, to enable,
*             GT_FALSE, otherwise.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM    - on bad parameter
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetCTTooEarlyIntEn
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL        en
);

/*******************************************************************************
* gsysGetCTTooEarlyIntEn
*
* DESCRIPTION:
*    Get Cut Through Point Too Early Interrupt Enable. When this bit is set to a
*    one the CTTooEarly bit (above) being a one will generate a Watch Dog
*    interrupt (Global 2 offset 0x00 and Global 2 offset 0x1B). When this bit is
*    cleared to a zero, the CTTooEarly bit being a one will not generate a
*    Watch Dog interrupt.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        en - GT_TRUE, to enable,
*             GT_FALSE, otherwise.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM    - on bad parameter
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysGetCTTooEarlyIntEn
(
    IN GT_QD_DEV    *dev,
    OUT GT_BOOL     *en
);

/*******************************************************************************
* gsysSetCutThroughPoint
*
* DESCRIPTION:
*    Set Cut Through Point in the frame. When Cut Through is enabled on a port
*    (Port offset 0x1F) this is the point in the frame where Cut Through is
*    attempted. A value of 0x00 is at byte 64, the default. The Cut Through
*    Point can be made earlier, but values between 0x01 and 0x18 (24
*    decimal) are ignored and result in a Cut Through Point at byte 64.
*
* INPUTS:
*        pooint - Cut Through Point in the frame.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM    - on bad parameter
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetCutThroughPoint
(
    IN GT_QD_DEV    *dev,
    IN GT_U8        point
);

/*******************************************************************************
* gsysGetCutThroughPoint
*
* DESCRIPTION:
*    Get Cut Through Point in the frame. When Cut Through is enabled on a port
*    (Port offset 0x1F) this is the point in the frame where Cut Through is
*    attempted. A value of 0x00 is at byte 64, the default. The Cut Through
*    Point can be made earlier, but values between 0x01 and 0x18 (24
*    decimal) are ignored and result in a Cut Through Point at byte 64.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        pooint - Cut Through Point in the frame.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM    - on bad parameter
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysGetCutThroughPoint
(
    IN GT_QD_DEV    *dev,
    OUT GT_U8       *point
);



/*******************************************************************************
* gsysReadMiiRegister
*
* DESCRIPTION:
*       This routine reads QuarterDeck Registers. Since this routine is only for
*        Diagnostic Purpose, no error checking will be performed.
*        User has to know exactly which phy address(0 ~ 0x1F) will be read.
*
* INPUTS:
*       phyAddr - Phy Address to read the register for.( 0 ~ 0x1F )
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysReadMiiReg
(
    IN GT_QD_DEV* dev,
    IN  GT_U32    phyAddr,
    IN  GT_U32    regAddr,
    OUT GT_U32    *data
);

/*******************************************************************************
* gsysWriteMiiRegister
*
* DESCRIPTION:
*       This routine writes QuarterDeck Registers. Since this routine is only for
*        Diagnostic Purpose, no error checking will be performed.
*        User has to know exactly which phy address(0 ~ 0x1F) will be read.
*
* INPUTS:
*       phyAddr - Phy Address to read the register for.( 0 ~ 0x1F )
*       regAddr - The register's address.
*       data    - data to be written.
*
* OUTPUTS:
*        None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysWriteMiiReg
(
    IN GT_QD_DEV* dev,
    IN  GT_U32    phyAddr,
    IN  GT_U32    regAddr,
    IN  GT_U16    data
);

#ifdef GT_RMGMT_ACCESS
/*******************************************************************************
* gsysAccessMultiRegs
*
* DESCRIPTION:
*       This function accesses switch's registers.
*
* INPUTS:
*   regList     - list of HW_DEV_RW_REG.
*     HW_DEV_RW_REG:
*     cmd - HW_REG_READ, HW_REG_WRITE, HW_REG_WAIT_TILL_0 or HW_REG_WAIT_TILL_1 
*     addr - SMI Address 
*     reg  - Register offset 
*     data - INPUT,OUTPUT:Value in the Register or Bit number
*     
* OUTPUTS:
*   regList
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysAccessMultiRegs
(
    IN  GT_QD_DEV    *dev,
    INOUT HW_DEV_REG_ACCESS *regList
);
#endif

/*******************************************************************************
* gsysGetSW_Mode
*
* DESCRIPTION:
*       This routine get the Switch mode. These two bits returen 
*       the current value of the SW_MODE[1:0] pins.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       mode - GT_TRUE Discard is enabled, GT_FALSE otherwise.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on bad parameter
*       GT_FAIL         - on error
*
* COMMENTS:
*         This feature is for both clippership and fullsail
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetSW_Mode
(
    IN GT_QD_DEV* dev,
    IN GT_SW_MODE *mode
);

/*******************************************************************************
* gsysGetInitReady
*
* DESCRIPTION:
*       This routine get the InitReady bit. This bit is set to a one when the ATU,
*       the Queue Controller and the Statistics Controller are done with their 
*       initialization and are ready to accept frames.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       mode - GT_TRUE: switch is ready, GT_FALSE otherwise.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on bad parameter
*       GT_FAIL         - on error
*
* COMMENTS:
*         This feature is for both clippership and fullsail
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetInitReady
(
    IN GT_QD_DEV* dev,
    IN GT_BOOL *mode
);


/*******************************************************************************
* gstatsFlushAll
*
* DESCRIPTION:
*       Flush All RMON counters for all ports.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*
* COMMENTS:
*       None
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gstatsFlushAll
(
    IN GT_QD_DEV* dev
);

/*******************************************************************************
* gstatsFlushPort
*
* DESCRIPTION:
*       Flush All RMON counters for a given port.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*
* COMMENTS:
*
*       None
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gstatsFlushPort
(
    IN GT_QD_DEV* dev,
    IN GT_LPORT    port
);

/*******************************************************************************
* gstatsGetPortCounter
*
* DESCRIPTION:
*        This routine gets a specific counter of the given port
*
* INPUTS:
*        port - the logical port number.
*        counter - the counter which will be read
*
* OUTPUTS:
*        statsData - points to 32bit data storage for the MIB counter
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*
* COMMENTS:
*        None
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gstatsGetPortCounter
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT        port,
    IN  GT_STATS_COUNTERS    counter,
    OUT GT_U32            *statsData
);

/*******************************************************************************
* gstatsGetPortAllCounters
*
* DESCRIPTION:
*       This routine gets all RMON counters of the given port
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       statsCounterSet - points to GT_STATS_COUNTER_SET for the MIB counters
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*
* COMMENTS:
*       None
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gstatsGetPortAllCounters
(
    IN  GT_QD_DEV* dev,
    IN  GT_LPORT        port,
    OUT GT_STATS_COUNTER_SET    *statsCounterSet
);


/*******************************************************************************
* grcSetLimitMode
*
* DESCRIPTION:
*       This routine sets the port's rate control ingress limit mode.
*
* INPUTS:
*       port    - logical port number.
*       mode     - rate control ingress limit mode. 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_BAD_PARAM        - on bad parameters
*******************************************************************************/
GT_STATUS grcSetLimitMode
(
    IN GT_QD_DEV*            dev,
    IN GT_LPORT          port,
    IN GT_RATE_LIMIT_MODE    mode
);

/*******************************************************************************
* grcGetLimitMode
*
* DESCRIPTION:
*       This routine gets the port's rate control ingress limit mode.
*
* INPUTS:
*       port    - logical port number.
*
* OUTPUTS:
*       mode     - rate control ingress limit mode. 
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_BAD_PARAM        - on bad parameters
*
*******************************************************************************/
GT_STATUS grcGetLimitMode
(
    IN GT_QD_DEV* dev,
    IN  GT_LPORT port,
    OUT GT_RATE_LIMIT_MODE    *mode
);

/*******************************************************************************
* grcSetPri3Rate
*
* DESCRIPTION:
*       This routine sets the ingress data rate limit for priority 3 frames.
*       Priority 3 frames will be discarded after the ingress rate selection
*       is reached or exceeded.
*
* INPUTS:
*       port - the logical port number.
*       mode - the priority 3 frame rate limit mode
*              GT_FALSE: use the same rate as Pri2Rate
*              GT_TRUE:  use twice the rate as Pri2Rate
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS grcSetPri3Rate
(
    IN GT_QD_DEV*            dev,
    IN GT_LPORT port,
    IN GT_BOOL  mode
);

/*******************************************************************************
* grcGetPri3Rate
*
* DESCRIPTION:
*       This routine gets the ingress data rate limit for priority 3 frames.
*       Priority 3 frames will be discarded after the ingress rate selection
*       is reached or exceeded.
*
* INPUTS:
*       port - the logical port number.
*       
* OUTPUTS:
*       mode - the priority 3 frame rate limit mode
*              GT_FALSE: use the same rate as Pri2Rate
*              GT_TRUE:  use twice the rate as Pri2Rate
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_BAD_PARAM        - on bad parameters
*
*******************************************************************************/
GT_STATUS grcGetPri3Rate
(
    IN GT_QD_DEV* dev,
    IN  GT_LPORT port,
    OUT GT_BOOL  *mode
);

/*******************************************************************************
* grcSetPri2Rate
*
* DESCRIPTION:
*       This routine sets the ingress data rate limit for priority 2 frames.
*       Priority 2 frames will be discarded after the ingress rate selection
*       is reached or exceeded.
*
* INPUTS:
*       port - the logical port number.
*       mode - the priority 2 frame rate limit mode
*              GT_FALSE: use the same rate as Pri1Rate
*              GT_TRUE:  use twice the rate as Pri1Rate
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS grcSetPri2Rate
(
    IN GT_QD_DEV*            dev,
    IN GT_LPORT port,
    IN GT_BOOL  mode
);

/*******************************************************************************
* grcGetPri2Rate
*
* DESCRIPTION:
*       This routine gets the ingress data rate limit for priority 2 frames.
*       Priority 2 frames will be discarded after the ingress rate selection
*       is reached or exceeded.
*
* INPUTS:
*       port - the logical port number.
*       
* OUTPUTS:
*       mode - the priority 2 frame rate limit mode
*              GT_FALSE: use the same rate as Pri1Rate
*              GT_TRUE:  use twice the rate as Pri1Rate
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_BAD_PARAM        - on bad parameters
*
*******************************************************************************/
GT_STATUS grcGetPri2Rate
(
    IN GT_QD_DEV*            dev,
    IN  GT_LPORT port,
    OUT GT_BOOL  *mode
);

/*******************************************************************************
* grcSetPri1Rate
*
* DESCRIPTION:
*       This routine sets the ingress data rate limit for priority 1 frames.
*       Priority 1 frames will be discarded after the ingress rate selection
*       is reached or exceeded.
*
* INPUTS:
*       port - the logical port number.
*       mode - the priority 1 frame rate limit mode
*              GT_FALSE: use the same rate as Pri0Rate
*              GT_TRUE:  use twice the rate as Pri0Rate
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS grcSetPri1Rate
(
    IN GT_QD_DEV*            dev,
    IN GT_LPORT port,
    IN GT_BOOL  mode
);

/*******************************************************************************
* grcGetPri1Rate
*
* DESCRIPTION:
*       This routine gets the ingress data rate limit for priority 1 frames.
*       Priority 1 frames will be discarded after the ingress rate selection
*       is reached or exceeded.
*
* INPUTS:
*       port - the logical port number.
*       
* OUTPUTS:
*       mode - the priority 1 frame rate limit mode
*              GT_FALSE: use the same rate as Pri0Rate
*              GT_TRUE:  use twice the rate as Pri0Rate
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_BAD_PARAM        - on bad parameters
*
*******************************************************************************/
GT_STATUS grcGetPri1Rate
(
    IN GT_QD_DEV*            dev,
    IN  GT_LPORT port,
    OUT GT_BOOL  *mode
);

/*******************************************************************************
* grcSetPri0Rate
*
* DESCRIPTION:
*       This routine sets the port's ingress data limit for priority 0 frames.
*
* INPUTS:
*       port    - logical port number.
*       rate    - ingress data rate limit for priority 0 frames. These frames
*             will be discarded after the ingress rate selected is reached 
*             or exceeded. 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_BAD_PARAM        - on bad parameters
*
*******************************************************************************/
GT_STATUS grcSetPri0Rate
(
    IN GT_QD_DEV*            dev,
    IN GT_LPORT        port,
    IN GT_PRI0_RATE    rate
);

/*******************************************************************************
* grcGetPri0Rate
*
* DESCRIPTION:
*       This routine gets the port's ingress data limit for priority 0 frames.
*
* INPUTS:
*       port    - logical port number to set.
*
* OUTPUTS:
*       rate    - ingress data rate limit for priority 0 frames. These frames
*             will be discarded after the ingress rate selected is reached 
*             or exceeded. 
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_BAD_PARAM        - on bad parameters
*
*******************************************************************************/
GT_STATUS grcGetPri0Rate
(
    IN GT_QD_DEV*            dev,
    IN  GT_LPORT port,
    OUT GT_PRI0_RATE    *rate
);

/*******************************************************************************
* grcSetBytesCount
*
* DESCRIPTION:
*       This routine sets the byets to count for limiting needs to be determined
*
* INPUTS:
*       port      - logical port number to set.
*        limitMGMT - GT_TRUE: To limit and count MGMT frame bytes
*                GT_FALSE: otherwise
*        countIFG  - GT_TRUE: To count IFG bytes
*                GT_FALSE: otherwise
*        countPre  - GT_TRUE: To count Preamble bytes
*                GT_FALSE: otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_BAD_PARAM        - on bad parameters
*
*******************************************************************************/
GT_STATUS grcSetBytesCount
(
    IN GT_QD_DEV*       dev,
    IN GT_LPORT            port,
    IN GT_BOOL         limitMGMT,
    IN GT_BOOL         countIFG,
    IN GT_BOOL         countPre
);

/*******************************************************************************
* grcGetBytesCount
*
* DESCRIPTION:
*       This routine gets the byets to count for limiting needs to be determined
*
* INPUTS:
*       port    - logical port number 
*
* OUTPUTS:
*        limitMGMT - GT_TRUE: To limit and count MGMT frame bytes
*                GT_FALSE: otherwise
*        countIFG  - GT_TRUE: To count IFG bytes
*                GT_FALSE: otherwise
*        countPre  - GT_TRUE: To count Preamble bytes
*                GT_FALSE: otherwise
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_BAD_PARAM        - on bad parameters
*
*******************************************************************************/
GT_STATUS grcGetBytesCount
(
    IN GT_QD_DEV*       dev,
    IN GT_LPORT            port,
    IN GT_BOOL         *limitMGMT,
    IN GT_BOOL         *countIFG,
    IN GT_BOOL         *countPre
);

/*******************************************************************************
* grcSetEgressRate
*
* DESCRIPTION:
*       This routine sets the port's egress data limit.
*        
*
* INPUTS:
*       port      - logical port number.
*       rateType  - egress data rate limit (GT_ERATE_TYPE union type). 
*                    union type is used to support multiple devices with the
*                    different formats of egress rate.
*                    GT_ERATE_TYPE has the following fields:
*                        definedRate - GT_EGRESS_RATE enum type should used for the 
*                            following devices:
*                            88E6218, 88E6318, 88E6063, 88E6083, 88E6181, 88E6183,
*                            88E6093, 88E6095, 88E6185, 88E6108, 88E6065, 88E6061, 
*                            and their variations
*                        kbRate - rate in kbps that should used for the following 
*                            devices:
*                            88E6097, 88E6096 with the GT_PIRL_ELIMIT_MODE of 
*                                GT_PIRL_ELIMIT_LAYER1,
*                                GT_PIRL_ELIMIT_LAYER2, or 
*                                GT_PIRL_ELIMIT_LAYER3 (see grcSetELimitMode)
*                            64kbps ~ 1Mbps    : increments of 64kbps,
*                            1Mbps ~ 100Mbps   : increments of 1Mbps, and
*                            100Mbps ~ 1000Mbps: increments of 10Mbps
*                            1Gbps ~ 10Gbps: increments of 100Mbps
*                            Therefore, the valid values are:
*                                64, 128, 192, 256, 320, 384,..., 960,
*                                1000, 2000, 3000, 4000, ..., 100000,
*                                110000, 120000, 130000, ..., 1000000, 10000000(for Opus).
*                        fRate - frame per second that should used for the following
*                            devices:
*                            88E6097, 88E6096 with GT_PIRL_ELIMIT_MODE of 
*                                GT_PIRL_ELIMIT_FRAME
*                            Valid values are between 7600 and 1488000 (14880000 for Opus)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_BAD_PARAM        - on bad parameters
*
* COMMENTS: 
*            GT_16M, GT_32M, GT_64M, GT_128M, and GT_256M in GT_EGRESS_RATE enum
*            are supported only by Gigabit Ethernet Switch.
*
*******************************************************************************/
GT_STATUS grcSetEgressRate
(
    IN GT_QD_DEV       *dev,
    IN GT_LPORT        port,
    IN GT_ERATE_TYPE   *rateType
);

/*******************************************************************************
* grcGetEgressRate
*
* DESCRIPTION:
*       This routine gets the port's egress data limit.
*
* INPUTS:
*       port    - logical port number.
*
* OUTPUTS:
*       rateType  - egress data rate limit (GT_ERATE_TYPE union type). 
*                    union type is used to support multiple devices with the
*                    different formats of egress rate.
*                    GT_ERATE_TYPE has the following fields:
*                        definedRate - GT_EGRESS_RATE enum type should used for the 
*                            following devices:
*                            88E6218, 88E6318, 88E6063, 88E6083, 88E6181, 88E6183,
*                            88E6093, 88E6095, 88E6185, 88E6108, 88E6065, 88E6061, 
*                            and their variations
*                        kbRate - rate in kbps that should used for the following 
*                            devices:
*                            88E6097, 88E6096 with the GT_PIRL_ELIMIT_MODE of 
*                                GT_PIRL_ELIMIT_LAYER1,
*                                GT_PIRL_ELIMIT_LAYER2, or 
*                                GT_PIRL_ELIMIT_LAYER3 (see grcSetELimitMode)
*                            64kbps ~ 1Mbps    : increments of 64kbps,
*                            1Mbps ~ 100Mbps   : increments of 1Mbps, and
*                            100Mbps ~ 1000Mbps: increments of 10Mbps
*                            1Gbps ~ 10Gbps: increments of 100Mbps
*                            Therefore, the valid values are:
*                                64, 128, 192, 256, 320, 384,..., 960,
*                                1000, 2000, 3000, 4000, ..., 100000,
*                                110000, 120000, 130000, ..., 1000000, 10000000(for Opus)
*                        fRate - frame per second that should used for the following
*                            devices:
*                            88E6097, 88E6096 with GT_PIRL_ELIMIT_MODE of 
*                                GT_PIRL_ELIMIT_FRAME
*                            Valid values are between 7600 and 1488000 (14880000 for Opus)
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_BAD_PARAM        - on bad parameters
*
* COMMENTS:
*            GT_16M, GT_32M, GT_64M, GT_128M, and GT_256M in GT_EGRESS_RATE enum
*            are supported only by Gigabit Ethernet Switch.
*
*******************************************************************************/
GT_STATUS grcGetEgressRate
(
    IN GT_QD_DEV *dev,
    IN  GT_LPORT port,
    OUT GT_ERATE_TYPE  *rateType
);

/*******************************************************************************
* gpavSetPAV
*
* DESCRIPTION:
*       This routine sets the Port Association Vector 
*
* INPUTS:
*       port    - logical port number.
*       pav     - Port Association Vector 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_BAD_PARAM        - on bad parameters
*
*******************************************************************************/
GT_STATUS gpavSetPAV
(
    IN GT_QD_DEV*       dev,
    IN GT_LPORT    port,
    IN GT_U16    pav
);

/*******************************************************************************
* gpavGetPAV
*
* DESCRIPTION:
*       This routine gets the Port Association Vector 
*
* INPUTS:
*       port    - logical port number.
*
* OUTPUTS:
*       pav     - Port Association Vector 
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_BAD_PARAM        - on bad parameters
*
*******************************************************************************/
GT_STATUS gpavGetPAV
(
    IN GT_QD_DEV*       dev,
    IN  GT_LPORT port,
    OUT GT_U16    *pav
);

/*******************************************************************************
* gpavSetIngressMonitor
*
* DESCRIPTION:
*       This routine sets the Ingress Monitor bit in the PAV.
*
* INPUTS:
*       port - the logical port number.
*       mode - the ingress monitor bit in the PAV
*              GT_FALSE: Ingress Monitor enabled 
*              GT_TRUE:  Ingress Monitor disabled 
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*******************************************************************************/
GT_STATUS gpavSetIngressMonitor
(
    IN GT_QD_DEV*       dev,
    IN GT_LPORT port,
    IN GT_BOOL  mode
);

/*******************************************************************************
* gpavGetIngressMonitor
*
* DESCRIPTION:
*       This routine gets the Ingress Monitor bit in the PAV.
*
* INPUTS:
*       port - the logical port number.
*       
* OUTPUTS:
*       mode - the ingress monitor bit in the PAV
*              GT_FALSE: Ingress Monitor enabled 
*              GT_TRUE:  Ingress Monitor disabled 
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_BAD_PARAM        - on bad parameters
*
*******************************************************************************/
GT_STATUS gpavGetIngressMonitor
(
    IN GT_QD_DEV*       dev,
    IN  GT_LPORT port,
    OUT GT_BOOL  *mode
);

/*******************************************************************************
* gvctGetCableStatus
*
* DESCRIPTION:
*       This routine perform the virtual cable test for the requested port,
*       and returns the the status per MDI pair.
*
* INPUTS:
*       port - logical port number.
*
* OUTPUTS:
*       cableStatus - the port copper cable status.
*       cableLen    - the port copper cable length.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_SUPPORTED- if current device does not support this feature.
*
* COMMENTS:
*       Internal Gigabit Phys in 88E6165 family and 88E6351 family devices
*        are not supported by this API. For those devices, gvctGetAdvCableDiag 
*        API can be used, instead.
*

*******************************************************************************/
GT_STATUS gvctGetCableDiag
(
    IN GT_QD_DEV*       dev,
    IN  GT_LPORT        port,
    OUT GT_CABLE_STATUS *cableStatus
);


/*******************************************************************************
* gvctGet1000BTExtendedStatus
*
* DESCRIPTION:
*       This routine retrieves extended cable status, such as Pair Poloarity,
*        Pair Swap, and Pair Skew. Note that this routine will be success only
*        if 1000Base-T Link is up.
*
* INPUTS:
*       port - logical port number.
*
* OUTPUTS:
*       extendedStatus - the extended cable status.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_SUPPORTED- if current device does not support this feature.
* COMMENTS:
*       Internal Gigabit Phys in 88E6165 family and 88E6351 family devices
*        are not supported by this API. For those devices, gvctGetAdvExtendedStatus 
*        API can be used, instead.
*
*
*******************************************************************************/
GT_STATUS gvctGet1000BTExtendedStatus
(
    IN  GT_QD_DEV         *dev,
    IN  GT_LPORT        port,
    OUT GT_1000BT_EXTENDED_STATUS *extendedStatus
);


/*******************************************************************************
* gtMemSet
*
* DESCRIPTION:
*       Set a block of memory
*
* INPUTS:
*       start  - start address of memory block for setting
*       simbol - character to store, converted to an unsigned char
*       size   - size of block to be set
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Pointer to set memory block
*
* COMMENTS:
*       None
*
*******************************************************************************/
void * gtMemSet
(
    IN void * start,
    IN int    symbol,
    IN GT_U32 size
);

/*******************************************************************************
* gtMemCpy
*
* DESCRIPTION:
*       Copies 'size' characters from the object pointed to by 'source' into
*       the object pointed to by 'destination'. If copying takes place between
*       objects that overlap, the behavior is undefined.
*
* INPUTS:
*       destination - destination of copy
*       source      - source of copy
*       size        - size of memory to copy
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Pointer to destination
*
* COMMENTS:
*       None
*
*******************************************************************************/
void * gtMemCpy
(
    IN void *       destination,
    IN const void * source,
    IN GT_U32       size
);


/*******************************************************************************
* gtMemCmp
*
* DESCRIPTION:
*       Compares given memories.
*
* INPUTS:
*       src1 - source 1
*       src2 - source 2
*       size - size of memory to copy
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0, if equal.
*        negative number, if src1 < src2.
*        positive number, if src1 > src2.
*
* COMMENTS:
*       None
*
*******************************************************************************/
int gtMemCmp
(
    IN char src1[],
    IN char src2[],
    IN GT_U32 size
);

/*******************************************************************************
* gtStrlen
*
* DESCRIPTION:
*       Determine the length of a string
* INPUTS:
*       source  - string
*
* OUTPUTS:
*       None
*
* RETURNS:
*       size    - number of characters in string, not including EOS.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_U32 gtStrlen
(
    IN const void * source
);

/*******************************************************************************
* gtDelay
*
* DESCRIPTION:
*       Wait for the given uSec and return.
*        Current Switch devices with Gigabit Ethernet Support require 250 uSec
*        of delay time for PPU to be disabled.
*        Since this function is System and/or OS dependent, it should be provided
*        by each DSDT user.
*
* INPUTS:
*       delayTime - delay in uSec.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
void gtDelay
(
    IN const unsigned int delayTime
);


/*******************************************************************************
* gtVersion
*
* DESCRIPTION:
*       This function returns the version of the QuarterDeck SW suite.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       version     - QuarterDeck software version.
*
* RETURNS:
*       GT_OK on success,
*       GT_BAD_PARAM on bad parameters,
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtVersion
(
    OUT GT_VERSION   *version
);


/* Prototypes added for Gigabit Ethernet Switch Support */


/* gtBrgFdb.c */

/*******************************************************************************
* gfdbMove
*
* DESCRIPTION:
*        This routine moves all or unblocked addresses from a port to another.
*
* INPUTS:
*        moveCmd  - the move operation type.
*        moveFrom - port where moving from
*        moveTo   - port where moving to
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK           - on success
*        GT_FAIL         - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gfdbMove
(
    IN GT_QD_DEV     *dev,
    IN GT_MOVE_CMD    moveCmd,
    IN GT_U32        moveFrom,
    IN GT_U32        moveTo
);

/*******************************************************************************
* gfdbMoveInDB
*
* DESCRIPTION:
*         This routine move all or unblocked addresses which are in the particular
*         ATU Database (DBNum) from a port to another.
*
* INPUTS:
*         moveCmd  - the move operation type.
*        DBNum         - ATU MAC Address Database Number.
*        moveFrom - port where moving from
*        moveTo   - port where moving to
*
* OUTPUTS:
*     None
*
* RETURNS:
*         GT_OK           - on success
*         GT_FAIL         - on error
*         GT_NOT_SUPPORTED- if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gfdbMoveInDB
(
    IN GT_QD_DEV   *dev,
    IN GT_MOVE_CMD moveCmd,
    IN GT_U32         DBNum,
    IN GT_U32        moveFrom,
    IN GT_U32        moveTo
);

/*******************************************************************************
* gfdbSetATUTestMode
*
* DESCRIPTION:
*       Set ATU test mode. When this bit is set to a one the divide by 16,384
*		Prescaler in the ATU Address Age Timers is bypassed accelerating the age
*		times.
*
* INPUTS:
*       mode - ATU test mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gfdbSetATUTestMode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_BOOL      mode
);

/*******************************************************************************
* gfdbGetATUTestMode
*
* DESCRIPTION:
*       Get ATU test mode. When this bit is set to a one the divide by 16,384
*		Prescaler in the ATU Address Age Timers is bypassed accelerating the age
*		times.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*		mode - ATU test mode.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gfdbGetATUTestMode
(
    IN   GT_QD_DEV    *dev,
    OUT  GT_BOOL      *mode
);

/*******************************************************************************
* gfdbSetMacFPri
*
* DESCRIPTION:
*       Set MAC frame priority bits. These bits are used to override the frame
*		priority on any frames associated with this MAC value, if the EntryState
*		bits indicate MAC priority can be used and the port's SA and/or DA priority
*		overrides are enabled for frame priority override.
*
* INPUTS:
*       pri - MAC frame priority bits.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gfdbSetMacFPri
(
    IN  GT_QD_DEV    *dev,
    IN  GT_U16       pri
);

/*******************************************************************************
* gfdbGetMacFPri
*
* DESCRIPTION:
*       Get MAC frame priority bits. These bits are used to override the frame
*		priority on any frames associated with this MAC value, if the EntryState
*		bits indicate MAC priority can be used and the port's SA and/or DA priority
*		overrides are enabled for frame priority override.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       pri - MAC frame priority bits.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gfdbGetMacFPri
(
    IN   GT_QD_DEV    *dev,
    OUT  GT_U16       *pri
);

/* gtBrgStp.c */

/* gtBrgVlan.c */

/* gtBrgVtu.c */

/* gtEvents.c */

/*******************************************************************************
* gatuGetIntStatus
*
* DESCRIPTION:
*        Check to see if a specific type of ATU interrupt occured
*
* INPUTS:
*     intType - the type of interrupt which causes an interrupt.
*                    GT_MEMEBER_VIOLATION, GT_MISS_VIOLATION, or GT_FULL_VIOLATION 
*
* OUTPUTS:
*         None.
*
* RETURNS:
*         GT_OK     - on success
*         GT_FAIL     - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gatuGetIntStatus
(
    IN  GT_QD_DEV                *dev,
    OUT GT_ATU_INT_STATUS    *atuIntStatus
);


/* gtPhyCtrl.c */

/*******************************************************************************
* gprtSet1000TMasterMode
*
* DESCRIPTION:
*        This routine set the port multicast rate limit.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_1000T_MASTER_SLAVE structure
*                autoConfig   - GT_TRUE for auto, GT_FALSE for manual setup.
*                masterPrefer - GT_TRUE if Master configuration is preferred.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSet1000TMasterMode
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_1000T_MASTER_SLAVE    *mode
);

/*******************************************************************************
* gprtGet1000TMasterMode
*
* DESCRIPTION:
*        This routine set the port multicast rate limit.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        mode - GT_1000T_MASTER_SLAVE structure
*                autoConfig   - GT_TRUE for auto, GT_FALSE for manual setup.
*                masterPrefer - GT_TRUE if Master configuration is preferred.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGet1000TMasterMode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_1000T_MASTER_SLAVE    *mode
);


/* gtPortCtrl.c */

/*******************************************************************************
* gprtSetDropOnLock
*
* DESCRIPTION:
*        This routine set the Drop on Lock. When set to one, Ingress frames will
*        be discarded if their SA field is not in the ATU's address database.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE for Unknown SA drop or GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetDropOnLock
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port,
    IN GT_BOOL        mode
);

/*******************************************************************************
* gprtGetDropOnLock
*
* DESCRIPTION:
*        This routine gets DropOnLock mode.
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE: DropOnLock enabled,
*                 GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetDropOnLock
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *mode
);

/*Liu, W/R per port reg InitalPri (0x4)*/

/*******************************************************************************
* gprtSetInitialPri
*
* DESCRIPTION:
*       This routine set initial Priority assignment.
*       Each frame entering a port is assigned a Frame Priority(FPri) and 
*       a Queue Priority (QPri). This mode can be assigned as follows:
*            GT_INITIAL_PRI_DEFAULT
*               
*            GT_INITIAL_PRI_TAG
*                
*            GT_INITIAL_PRI_IP
*                
*            GT_INITIAL_PRI_TAG_IP
*                
*
* INPUTS:
*       port - the logical port number.
*       mode - GT_INITIAL_PRI type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtSetInitialPri
(
    IN  GT_QD_DEV   *dev,
    IN  GT_LPORT     port,
    IN  GT_INITIAL_PRI      mode
);

/*******************************************************************************
* gprtGetInitialPri
*
* DESCRIPTION:
*       This routine gets initial Priority assignment.
*       Each frame entering a port is assigned a Frame Priority(FPri) and 
*       a Queue Priority (QPri). This mode can be assigned as follows:
*            GT_INITIAL_PRI_DEFAULT
*               
*            GT_INITIAL_PRI_TAG
*                
*            GT_INITIAL_PRI_IP
*                
*            GT_INITIAL_PRI_TAG_IP
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - GT_INITIAL_PRI type
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtGetInitialPri
(
    IN  GT_QD_DEV   *dev,
    IN  GT_LPORT     port,
    OUT GT_INITIAL_PRI      *mode
);

/*******************************************************************************
* gprtSetDoubleTag
*
* DESCRIPTION:
*        This routine set the Ingress Double Tag Mode. When set to one, 
*        ingressing frames are examined to see if they contain an 802.3ac tag.
*        If they do, the tag is removed and then the frame is processed from
*        there (i.e., removed tag is ignored). Essentially, untagged frames
*        remain untagged, single tagged frames become untagged and double tagged
*        frames become single tagged.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE for DoulbeTag mode or GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetDoubleTag
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port,
    IN GT_BOOL        mode
);

/*******************************************************************************
* gprtGetDoubleTag
*
* DESCRIPTION:
*        This routine gets DoubleTag mode.
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE: DoubleTag enabled,
*                 GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetDoubleTag
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *mode
);

/*******************************************************************************
* gprtSetInterswitchPort
*
* DESCRIPTION:
*        This routine set Interswitch Port. When set to one, 
*        it indicates this port is a interswitch port used to communicated with
*        CPU or to cascade with another switch device.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE for Interswitch port or GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetInterswitchPort
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port,
    IN GT_BOOL        mode
);

/*******************************************************************************
* gprtGetInterswithPort
*
* DESCRIPTION:
*        This routine gets InterswitchPort.
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE: This port is interswitch port,
*                 GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetInterswitchPort
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *mode
);

/*******************************************************************************
* gprtSetLearnDisable
*
* DESCRIPTION:
*        This routine enables/disables automatic learning of new source MAC
*        addresses on the given port ingress
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE for disable or GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetLearnDisable
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port,
    IN GT_BOOL        mode
);


/*******************************************************************************
* gprtGetLearnDisable
*
* DESCRIPTION:
*        This routine gets LearnDisable setup
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE: Learning disabled on the given port ingress frames,
*                 GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetLearnDisable
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *mode
);

/*******************************************************************************
* gprtSetIgnoreFCS
*
* DESCRIPTION:
*        This routine sets FCS Ignore mode. When this bit is set to a one,
*        the last four bytes of frames received on this port are overwritten with
*        a good CRC and the frames will be accepted by the switch.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE for ignore FCS or GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetIgnoreFCS
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port,
    IN GT_BOOL         mode
);

/*******************************************************************************
* gprtGetIgnoreFCS
*
* DESCRIPTION:
*        This routine gets Ignore FCS setup
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE: Ignore FCS on the given port's ingress frames,
*                 GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetIgnoreFCS
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *mode
);

/*******************************************************************************
* gprtSetAllowedBad
*
* DESCRIPTION:
*        Allow receiving frames on this port with a bad FCS..
*
* INPUTS:
*        port - the logical port number.
*        allowed - GT_TRUE, frames receviced on this port with a CRC error are not 
*        discarded.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetAllowedBad
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port,
    IN GT_BOOL         allowed
);

/*******************************************************************************
* gprtGetAllowedBad
*
* DESCRIPTION:
*        This routine gets Ignore FCS setup
*
* DESCRIPTION:
*        Allow receiving frames on this port with a bad FCS..
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        allowed - GT_TRUE, frames receviced on this port with a CRC error are not 
*        discarded.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetAllowedBad
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *allowed
);

/*******************************************************************************
* gprtSetVTUPriOverride
*
* DESCRIPTION:
*        VTU Priority Override. The following modes are supported:
*            PRI_OVERRIDE_NONE -
*                Normal frame priority processing occurs.
*            PRI_OVERRIDE_FRAME_QUEUE -
*                Both frame and queue overrides take place on the frame.
*            PRI_OVERRIDE_FRAME -
*                Priority assigned to the frame's VID (in the VTU table) is used
*                to overwite the frame's FPri (frame priority).
*                If the frame egresses tagged, the priority in the frame will be
*                this new priority value.
*            PRI_OVERRIDE_QUEUE -
*                Priority assigned to the frame's VID (in the VTU table) is used
*                to overwite the frame's QPri (queue priority).
*                QPri is used internally to map the frame to one of the egress
*                queues inside the switch.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_PRI_OVERRIDE type
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if mode is invalid
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*        PRI_OVERRIDE_FRAME and PRI_OVERRIDE_QUEUE modes are supported only on
*        certain switch device. Please refer to the device datasheet.
*
*******************************************************************************/
GT_STATUS gprtSetVTUPriOverride
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port,
    IN GT_PRI_OVERRIDE        mode
);

/*******************************************************************************
* gprtGetVTUPriOverride
*
* DESCRIPTION:
*        VTU Priority Override. The following modes are supported:
*            PRI_OVERRIDE_NONE -
*                Normal frame priority processing occurs.
*            PRI_OVERRIDE_FRAME_QUEUE -
*                Both frame and queue overrides take place on the frame.
*            PRI_OVERRIDE_FRAME -
*                Priority assigned to the frame's VID (in the VTU table) is used
*                to overwite the frame's FPri (frame priority).
*                If the frame egresses tagged, the priority in the frame will be
*                this new priority value.
*            PRI_OVERRIDE_QUEUE -
*                Priority assigned to the frame's VID (in the VTU table) is used
*                to overwite the frame's QPri (queue priority).
*                QPri is used internally to map the frame to one of the egress
*                queues inside the switch.
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode - GT_PRI_OVERRIDE type
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        PRI_OVERRIDE_FRAME and PRI_OVERRIDE_QUEUE modes are supported only on
*        certain switch device. Please refer to the device datasheet.
*
*******************************************************************************/
GT_STATUS gprtGetVTUPriOverride
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_PRI_OVERRIDE        *mode
);

/*******************************************************************************
* gprtSetSAPriOverride
*
* DESCRIPTION:
*        SA Priority Override. The following mode is supported:
*            PRI_OVERRIDE_NONE -
*                Normal frame priority processing occurs.
*            PRI_OVERRIDE_FRAME_QUEUE -
*                Both frame and queue overrides take place on the frame.
*            PRI_OVERRIDE_FRAME -
*                Priority assigned to the frame's SA (in the ATU table) is used
*                to overwite the frame's FPri (frame priority).
*                If the frame egresses tagged, the priority in the frame will be
*                this new priority value.
*            PRI_OVERRIDE_QUEUE -
*                Priority assigned to the frame's SA (in the ATU table) is used
*                to overwite the frame's QPri (queue priority).
*                QPri is used internally to map the frame to one of the egress
*                queues inside the switch.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_PRI_OVERRIDE type
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if mode is invalid
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*        PRI_OVERRIDE_FRAME and PRI_OVERRIDE_QUEUE modes are supported only on
*        certain switch device. Please refer to the device datasheet.
*
*******************************************************************************/
GT_STATUS gprtSetSAPriOverride
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port,
	IN GT_PRI_OVERRIDE mode
);

/*******************************************************************************
* gprtGetSAPriOverride
*
* DESCRIPTION:
*        SA Priority Override. The following mode is supported:
*            PRI_OVERRIDE_NONE -
*                Normal frame priority processing occurs.
*            PRI_OVERRIDE_FRAME_QUEUE -
*                Both frame and queue overrides take place on the frame.
*            PRI_OVERRIDE_FRAME -
*                Priority assigned to the frame's SA (in the ATU table) is used
*                to overwite the frame's FPri (frame priority).
*                If the frame egresses tagged, the priority in the frame will be
*                this new priority value.
*            PRI_OVERRIDE_QUEUE -
*                Priority assigned to the frame's SA (in the ATU table) is used
*                to overwite the frame's QPri (queue priority).
*                QPri is used internally to map the frame to one of the egress
*                queues inside the switch.
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode - GT_PRI_OVERRIDE type
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        PRI_OVERRIDE_FRAME and PRI_OVERRIDE_QUEUE modes are supported only on
*        certain switch device. Please refer to the device datasheet.
*
*******************************************************************************/
GT_STATUS gprtGetSAPriOverride
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_PRI_OVERRIDE        *mode
);

/*******************************************************************************
* gprtSetDAPriOverride
*
* DESCRIPTION:
*        DA Priority Override. The following mode is supported:
*            PRI_OVERRIDE_NONE -
*                Normal frame priority processing occurs.
*            PRI_OVERRIDE_FRAME -
*                Priority assigned to the frame's DA (in the ATU table) is used
*                to overwite the frame's FPri (frame priority).
*                If the frame egresses tagged, the priority in the frame will be
*                this new priority value.
*            PRI_OVERRIDE_QUEUE -
*                Priority assigned to the frame's DA (in the ATU table) is used
*                to overwite the frame's QPri (queue priority).
*                QPri is used internally to map the frame to one of the egress
*                queues inside the switch.
*            PRI_OVERRIDE_FRAME_QUEUE -
*                Both frame and queue overrides take place on the frame.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_PRI_OVERRIDE type
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if mode is invalid
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*        PRI_OVERRIDE_FRAME and PRI_OVERRIDE_QUEUE modes are supported only on
*        certain switch device. Please refer to the device datasheet.
*
*******************************************************************************/
GT_STATUS gprtSetDAPriOverride
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_PRI_OVERRIDE        mode
);

/*******************************************************************************
* gprtGetDAPriOverride
*
* DESCRIPTION:
*        DA Priority Override. The following mode is supported:
*            PRI_OVERRIDE_NONE -
*                Normal frame priority processing occurs.
*            PRI_OVERRIDE_FRAME -
*                Priority assigned to the frame's DA (in the ATU table) is used
*                to overwite the frame's FPri (frame priority).
*                If the frame egresses tagged, the priority in the frame will be
*                this new priority value.
*            PRI_OVERRIDE_QUEUE -
*                Priority assigned to the frame's DA (in the ATU table) is used
*                to overwite the frame's QPri (queue priority).
*                QPri is used internally to map the frame to one of the egress
*                queues inside the switch.
*            PRI_OVERRIDE_FRAME_QUEUE -
*                Both frame and queue overrides take place on the frame.
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode - GT_PRI_OVERRIDE type
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        PRI_OVERRIDE_FRAME and PRI_OVERRIDE_QUEUE modes are supported only on
*        certain switch device. Please refer to the device datasheet.
*
*******************************************************************************/
GT_STATUS gprtGetDAPriOverride
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_PRI_OVERRIDE        *mode
);

/*******************************************************************************
* gprtSetCPUPort
*
* DESCRIPTION:
*        This routine sets CPU Port number. When Snooping is enabled on this port
*        or when this port is configured as an Interswitch Port and it receives a 
*        To_CPU frame, the switch needs to know what port on this device the frame 
*        should egress.
*
* INPUTS:
*        port - the logical port number.
*        cpuPort - CPU Port number or interswitch port where CPU Port is connected
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetCPUPort
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_LPORT     cpuPort
);

/*******************************************************************************
* gprtGetCPUPort
*
* DESCRIPTION:
*        This routine gets CPU Logical Port
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        cpuPort - CPU Port's logical number
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetCPUPort
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_LPORT     *cpuLPort
);

/*******************************************************************************
* gprtSetLockedPort
*
* DESCRIPTION:
*        This routine sets LockedPort. When it's set to one, CPU directed 
*        learning for 802.1x MAC authentication is enabled on this port. In this
*        mode, an ATU Miss Violation interrupt will occur when a new SA address
*        is received in a frame on this port. Automatically SA learning and 
*        refreshing is disabled in this mode.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE for Locked Port, GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetLockedPort
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL        mode
);

/*******************************************************************************
* gprtGetLockedPort
*
* DESCRIPTION:
*        This routine gets Locked Port mode for the given port
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode  - GT_TRUE if LockedPort, GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetLockedPort
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL      *mode
);

/*******************************************************************************
* gprtSetIgnoreWrongData
*
* DESCRIPTION:
*        This routine sets Ignore Wrong Data. If the frame's SA address is found 
*        in the database and if the entry is 'static' or if the port is 'locked'
*        the source port's bit is checked to insure the SA has been assigned to 
*        this port. If the SA is NOT assigned to this port, it is considered an 
*        ATU Member Violation. If the IgnoreWrongData is set to GT_FALSE, an ATU
*        Member Violation interrupt will be generated. If it's set to GT_TRUE,
*        the ATU Member Violation error will be masked and ignored.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE for IgnoreWrongData, GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetIgnoreWrongData
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port,
    IN GT_BOOL        mode
);


/*******************************************************************************
* gprtGetIgnoreWrongData
*
* DESCRIPTION:
*        This routine gets Ignore Wrong Data mode for the given port
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode  - GT_TRUE if IgnoreWrongData, GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetIgnoreWrongData
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL     *mode
);


/* gtPortRateCtrl.c */

/* gtPortRmon.c */

/*******************************************************************************
* gstatsGetPortCounter2
*
* DESCRIPTION:
*        This routine gets a specific counter of the given port
*
* INPUTS:
*        port - the logical port number.
*        counter - the counter which will be read
*
* OUTPUTS:
*        statsData - points to 32bit data storage for the MIB counter
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*
* COMMENTS:
*        This function supports Gigabit Switch only
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gstatsGetPortCounter2
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT        port,
    IN  GT_STATS_COUNTERS2    counter,
    OUT GT_U32            *statsData
);


/*******************************************************************************
* gstatsGetPortAllCounters2
*
* DESCRIPTION:
*        This routine gets all counters of the given port
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        statsCounterSet - points to GT_STATS_COUNTER_SET for the MIB counters
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*
* COMMENTS:
*        This function supports Gigabit Switch only
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gstatsGetPortAllCounters2
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT        port,
    OUT GT_STATS_COUNTER_SET2    *statsCounterSet
);

/*******************************************************************************
* gstatsGetHistogramMode
*
* DESCRIPTION:
*        This routine gets the Histogram Counters Mode.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        mode - Histogram Mode (GT_COUNT_RX_ONLY, GT_COUNT_TX_ONLY, 
*                    and GT_COUNT_RX_TX)
*
* RETURNS:
*        GT_OK           - on success
*        GT_BAD_PARAM    - on bad parameter
*        GT_FAIL         - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        This function supports Gigabit Switch only
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gstatsGetHistogramMode
(
    IN  GT_QD_DEV                *dev,
    OUT GT_HISTOGRAM_MODE    *mode
);

/*******************************************************************************
* gstatsSetHistogramMode
*
* DESCRIPTION:
*        This routine sets the Histogram Counters Mode.
*
* INPUTS:
*        mode - Histogram Mode (GT_COUNT_RX_ONLY, GT_COUNT_TX_ONLY, 
*                    and GT_COUNT_RX_TX)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK           - on success
*        GT_BAD_PARAM    - on bad parameter
*        GT_FAIL         - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gstatsSetHistogramMode
(
    IN GT_QD_DEV                 *dev,
    IN GT_HISTOGRAM_MODE        mode
);


/* gtPortStatus.c */

/*******************************************************************************
* gprtGetPauseEn
*
* DESCRIPTION:
*        This routine retrives the link pause state.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE for enable or GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        If set MAC Pause (for Full Duplex flow control) is implemented in the
*        link partner and in MyPause
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetPauseEn
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL     *state
);

/*******************************************************************************
* gprtGetHdFlow
*
* DESCRIPTION:
*        This routine retrives the half duplex flow control value.
*        If set, Half Duplex back pressure will be used on this port if this port
*        is in a half duplex mode.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE for enable or GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetHdFlow
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL     *state
);

/*******************************************************************************
* gprtGetPHYDetect
*
* DESCRIPTION:
*        This routine retrives the information regarding PHY detection.
*        If set, An 802.3 PHY is attached to this port.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE if connected or GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetPHYDetect
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL     *state
);

/*******************************************************************************
* gprtSetPHYDetect
*
* DESCRIPTION:
*        This routine sets PHYDetect bit which make PPU change its polling.
*        PPU's pool routine uses these bits to determine which port's to poll
*        PHYs on for Link, Duplex, Speed, and Flow Control.
*
* INPUTS:
*        port - the logical port number.
*        state - GT_TRUE or GT_FALSE
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        This function should not be called if gsysGetPPUState returns 
*        PPU_STATE_ACTIVE.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetPHYDetect
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_BOOL      state
);

/*******************************************************************************
* gprtGetSpeedMode
*
* DESCRIPTION:
*       This routine retrives the port speed.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - GT_PORT_SPEED_MODE type.
*                    (PORT_SPEED_1000_MBPS,PORT_SPEED_100_MBPS, or PORT_SPEED_10_MBPS)
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetSpeedMode
(
    IN  GT_QD_DEV *dev,
    IN  GT_LPORT  port,
    OUT GT_PORT_SPEED_MODE   *speed
);

/*******************************************************************************
* gprtGetDuplexFixed
*
* DESCRIPTION:
*        This routine retrives the Duplex Fixed bit
*        When this bit is set to a one a Duplex Mismatch was detected on this port
*		 and its duplex has been AutoFixed.
*		 It will clear to zero when link from the MAC on this port is detected or
*		 forced down.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE or GT_FALSE
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetDuplexFixed
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
);

/*******************************************************************************
* gprtGetHighErrorRate
*
* DESCRIPTION:
*        This routine retrives the PCS High Error Rate.
*        This routine returns GT_TRUE if the rate of invalid code groups seen by
*        PCS has exceeded 10 to the power of -11.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE or GT_FALSE
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetHighErrorRate
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
);

/*******************************************************************************
* gprtGetTxPaused
*
* DESCRIPTION:
*        This routine retrives Transmit Pause state.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE if Rx MAC receives a PAUSE frame with none-zero Puase Time
*                  GT_FALSE otherwise.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetTxPaused
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
);


/*******************************************************************************
* gprtGetFlowCtrl
*
* DESCRIPTION:
*        This routine retrives Flow control state.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE if Rx MAC determines that no more data should be 
*                    entering this port.
*                  GT_FALSE otherwise.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetFlowCtrl
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
);

/*******************************************************************************
* gprtGetC_Duplex
*
* DESCRIPTION:
*        This routine retrives Port 9's duplex configuration mode determined
*        at reset.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE if configured as Full duplex operation
*                  GT_FALSE otherwise.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        Return value is valid only if the given port is 9.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetC_Duplex
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
);

/*******************************************************************************
* gprtGetC_Mode
*
* DESCRIPTION:
*        This routine retrives port's interface type configuration mode 
*        determined at reset.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - one of value in GT_PORT_CONFIG_MODE enum type
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        Return value is valid only if the given port is 9.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetC_Mode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_PORT_CONFIG_MODE   *state
);


/* gtSysCtrl.c */

/*******************************************************************************
* gsysSetPPUEn
*
* DESCRIPTION:
*        This routine enables/disables Phy Polling Unit.
*
* INPUTS:
*        en - GT_TRUE to enable PPU, GT_FALSE otherwise.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysSetPPUEn
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL         en
);

/*******************************************************************************
* gsysGetPPUEn
*
* DESCRIPTION:
*        This routine get the PPU state.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        en - GT_TRUE if PPU is enabled, GT_FALSE otherwise.
*
* RETURNS:
*        GT_OK           - on success
*        GT_BAD_PARAM    - on bad parameter
*        GT_FAIL         - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetPPUEn
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL      *en
);

/*******************************************************************************
* gsysSetCascadePort
*
* DESCRIPTION:
*        This routine sets Cascade Port number.
*        In multichip systems frames coming from a CPU need to know when they
*        have reached their destination chip.
*
* INPUTS:
*        port - Cascade Port
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysSetCascadePort
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port
);

/*******************************************************************************
* gsysGetCascadePort
*
* DESCRIPTION:
*        This routine gets Cascade Port number.
*        In multichip systems frames coming from a CPU need to know when they
*        have reached their destination chip.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        port - Cascade Port
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetCascadePort
(
    IN  GT_QD_DEV    *dev,
    OUT GT_LPORT     *port
);

/*******************************************************************************
* gsysSetDeviceNumber
*
* DESCRIPTION:
*        This routine sets Device Number.
*        In multichip systems frames coming from a CPU need to know when they
*        have reached their destination chip. From CPU frames whose Dev_Num
*        fieldmatches these bits have reachedtheir destination chip and are sent
*        out this chip using the port number indicated in the frame's Trg_Port 
*        field.
*
* INPUTS:
*        devNum - Device Number (0 ~ 31)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysSetDeviceNumber
(
    IN GT_QD_DEV    *dev,
    IN GT_U32          devNum
);

/*******************************************************************************
* gsysGetDeviceNumber
*
* DESCRIPTION:
*        This routine gets Device Number.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        devNum - Device Number (0 ~ 31)
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetDeviceNumber
(
    IN  GT_QD_DEV    *dev,
    OUT GT_U32      *devNum
);


/* gtPCSCtrl.c */


/*******************************************************************************
* gpcsGetCommaDet
*
* DESCRIPTION:
*        This routine retrieves Comma Detection status in PCS
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE for Comma Detected or GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetCommaDet
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
);

/*******************************************************************************
* gpcsGetSyncOK
*
* DESCRIPTION:
*        This routine retrieves SynOK bit. It is set to a one when the PCS has
*        detected a few comma patterns and is synchronized with its peer PCS 
*        layer.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE if synchronized or GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetSyncOK
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
);

/*******************************************************************************
* gpcsGetSyncFail
*
* DESCRIPTION:
*        This routine retrieves SynFail bit.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE if synchronizaion failed or GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetSyncFail
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
);

/*******************************************************************************
* gpcsGetAnBypassed
*
* DESCRIPTION:
*        This routine retrieves Inband Auto-Negotiation bypass status.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE if AN is bypassed or GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetAnBypassed
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
);

/*******************************************************************************
* gpcsGetAnBypassMode
*
* DESCRIPTION:
*        This routine retrieves Enable mode of Inband Auto-Negotiation bypass.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE if AN bypass is enabled or GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetAnBypassMode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *mode
);

/*******************************************************************************
* gpcsSetAnBypassMode
*
* DESCRIPTION:
*        This routine retrieves Enable mode of Inband Auto-Negotiation bypass.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE to enable AN bypass mode or GT_FALSE otherwise
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsSetAnBypassMode
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL      mode
);

/*******************************************************************************
* gpcsGetPCSAnEn
*
* DESCRIPTION:
*        This routine retrieves Enable mode of PCS Inband Auto-Negotiation.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE if PCS AN is enabled or GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetPCSAnEn
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *mode
);

/*******************************************************************************
* gpcsSetPCSAnEn
*
* DESCRIPTION:
*        This routine sets Enable mode of PCS Inband Auto-Negotiation.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE to enable PCS AN mode or GT_FALSE otherwise
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsSetPCSAnEn
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL      mode
);

/*******************************************************************************
* gpcsSetRestartPCSAn
*
* DESCRIPTION:
*        This routine restarts PCS Inband Auto-Negotiation.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsSetRestartPCSAn
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port
);

/*******************************************************************************
* gpcsGetPCSAnDone
*
* DESCRIPTION:
*        This routine retrieves completion information of PCS Auto-Negotiation.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE if PCS AN is done or never done
*                GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetPCSAnDone
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *mode
);

/*******************************************************************************
* gpcsSetLinkValue
*
* DESCRIPTION:
*        This routine sets Link's force value
*
* INPUTS:
*        port - the logical port number.
*        state - GT_TRUE to force link up, GT_FALSE otherwise
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsSetLinkValue
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN    GT_BOOL        state
);

/*******************************************************************************
* gpcsGetLinkValue
*
* DESCRIPTION:
*        This routine retrieves Link Value which will be used for Forcing Link 
*        up or down.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE if Link Force value is one (link up)
*                 GT_FALSE otherwise (link down)
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetLinkValue
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
);

/*******************************************************************************
* gpcsSetForcedLink
*
* DESCRIPTION:
*        This routine forces Link. If LinkValue is set to one, calling this 
*        routine with GT_TRUE will force Link to be up.
*
* INPUTS:
*        port - the logical port number.
*        state - GT_TRUE to force link (up or down), GT_FALSE otherwise
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsSetForcedLink
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN    GT_BOOL        state
);

/*******************************************************************************
* gpcsGetForcedLink
*
* DESCRIPTION:
*        This routine retrieves Forced Link bit
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE if ForcedLink bit is one,
*                 GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetForcedLink
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
);

/*******************************************************************************
* gpcsSetDpxValue
*
* DESCRIPTION:
*        This routine sets Duplex's Forced value. This function needs to be
*        called prior to gpcsSetForcedDpx.
*
* INPUTS:
*        port - the logical port number.
*        state - GT_TRUE to force full duplex, GT_FALSE otherwise
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsSetDpxValue
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN    GT_BOOL        state
);

/*******************************************************************************
* gpcsGetDpxValue
*
* DESCRIPTION:
*        This routine retrieves Duplex's Forced value
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE if Duplex's Forced value is set to Full duplex,
*                 GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetDpxValue
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
);

/*******************************************************************************
* gpcsSetForcedDpx
*
* DESCRIPTION:
*        This routine forces duplex mode. If DpxValue is set to one, calling this 
*        routine with GT_TRUE will force duplex mode to be full duplex.
*
* INPUTS:
*        port - the logical port number.
*        state - GT_TRUE to force duplex mode, GT_FALSE otherwise
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsSetForcedDpx
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN    GT_BOOL        state
);

/*******************************************************************************
* gpcsGetForcedDpx
*
* DESCRIPTION:
*        This routine retrieves Forced Duplex.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE if ForcedDpx bit is one,
*                 GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetForcedDpx
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
);

/*******************************************************************************
* gpcsSetForceSpeed
*
* DESCRIPTION:
*        This routine forces speed. 
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_PORT_FORCED_SPEED_MODE (10, 100, 1000, 10g,or No Speed Force)
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsSetForceSpeed
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN    GT_PORT_FORCED_SPEED_MODE  mode
);

/*******************************************************************************
* gpcsGetForceSpeed
*
* DESCRIPTION:
*        This routine retrieves Force Speed value
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_PORT_FORCED_SPEED_MODE (10, 100, 1000, 10g, or no force speed)
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetForceSpeed
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_PORT_FORCED_SPEED_MODE   *mode
);



/* gtQosMap.c */

/*******************************************************************************
* gqosGetTagRemap
*
* DESCRIPTION:
*        Gets the remapped priority value for a specific 802.1p priority on a
*        given port.
*
* INPUTS:
*        port  - the logical port number.
*        pri   - 802.1p priority
*
* OUTPUTS:
*        remappedPri - remapped Priority
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gqosGetTagRemap
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_U8        pri,
    OUT GT_U8       *remappedPri
);

/*******************************************************************************
* gqosSetTagRemap
*
* DESCRIPTION:
*        Sets the remapped priority value for a specific 802.1p priority on a
*        given port.
*
* INPUTS:
*        port  - the logical port number.
*        pri   - 802.1p priority
*        remappedPri - remapped Priority
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gqosSetTagRemap
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_U8        pri,
    IN GT_U8        remappedPri
);


/* gtSysConfig.c */

/* gtSysStatus.c */

/*******************************************************************************
* gsysGetPPUState
*
* DESCRIPTION:
*        This routine get the PPU State. These two bits return 
*        the current value of the PPU.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        mode - GT_PPU_STATE
*
* RETURNS:
*        GT_OK           - on success
*        GT_BAD_PARAM    - on bad parameter
*        GT_FAIL         - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetPPUState
(
    IN  GT_QD_DEV       *dev,
    OUT GT_PPU_STATE    *mode
);


/* Prototypes added for 88E6093 */


/* gtBrgFdb.c */

/*******************************************************************************
* gfdbGetLearn2All
*
* DESCRIPTION:
*        When more than one Marvell device is used to form a single 'switch', it
*        may be desirable for all devices in the 'switch' to learn any address this 
*        device learns. When this bit is set to a one all other devices in the 
*        'switch' learn the same addresses this device learns. When this bit is 
*        cleared to a zero, only the devices that actually receive frames will learn
*        from those frames. This mode typically supports more active MAC addresses 
*        at one time as each device in the switch does not need to learn addresses 
*        it may nerver use.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        mode  - GT_TRUE if Learn2All is enabled, GT_FALSE otherwise
*
* RETURNS:
*        GT_OK           - on success
*        GT_FAIL         - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gfdbGetLearn2All
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL     *mode
);

/*******************************************************************************
* gfdbSetLearn2All
*
* DESCRIPTION:
*        Enable or disable Learn2All mode.
*
* INPUTS:
*        mode - GT_TRUE to set Learn2All, GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gfdbSetLearn2All
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL        mode
);

/*******************************************************************************
* gfdbRemovePort
*
* DESCRIPTION:
*       This routine deassociages all or unblocked addresses from a port.
*
* INPUTS:
*       moveCmd - the move operation type.
*       port - the logical port number.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gfdbRemovePort
(
    IN GT_QD_DEV    *dev,
    IN GT_MOVE_CMD     moveCmd,
    IN GT_LPORT        port
);

/*******************************************************************************
* gfdbRemovePortInDB
*
* DESCRIPTION:
*       This routine deassociages all or unblocked addresses from a port in the
*       particular ATU Database (DBNum).
*
* INPUTS:
*       moveCmd  - the move operation type.
*       port - the logical port number.
*        DBNum     - ATU MAC Address Database Number.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_NOT_SUPPORTED- if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gfdbRemovePortInDB
(
    IN GT_QD_DEV    *dev,
    IN GT_MOVE_CMD     moveCmd,
    IN GT_LPORT        port,
    IN GT_U32         DBNum
);



/* gtBrgStp.c */

/* gtBrgVlan.c */

/* gtBrgVtu.c */

/* gtEvents.c */

/* gtPCSCtrl.c */

/*******************************************************************************
* gpcsGetPCSLink
*
* DESCRIPTION:
*        This routine retrieves Link up status in PCS
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE for Comma Detected or GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetPCSLink
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
);

/*******************************************************************************
* gpcsSetFCValue
*
* DESCRIPTION:
*        This routine sets Flow Control's force value
*
* INPUTS:
*        port - the logical port number.
*        state - GT_TRUE to force flow control enabled, GT_FALSE otherwise
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsSetFCValue
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN    GT_BOOL        state
);

/*******************************************************************************
* gpcsGetFCValue
*
* DESCRIPTION:
*        This routine retrieves Flow Control Value which will be used for Forcing 
*        Flow Control enabled or disabled.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE if FC Force value is one (flow control enabled)
*                 GT_FALSE otherwise (flow control disabled)
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetFCValue
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
);

/*******************************************************************************
* gpcsSetForcedFC
*
* DESCRIPTION:
*        This routine forces Flow Control. If FCValue is set to one, calling this 
*        routine with GT_TRUE will force Flow Control to be enabled.
*
* INPUTS:
*        port - the logical port number.
*        state - GT_TRUE to force flow control (enable or disable), GT_FALSE otherwise
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsSetForcedFC
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN    GT_BOOL        state
);

/*******************************************************************************
* gpcsGetForcedFC
*
* DESCRIPTION:
*        This routine retrieves Forced Flow Control bit
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE if ForcedFC bit is one,
*                 GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetForcedFC
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
);



/* gtPhyCtrl.c */

/*******************************************************************************
* gprtGetPagedPhyReg
*
* DESCRIPTION:
*       This routine reads phy register of the given page
*
* INPUTS:
*        port     - port to be read
*        regAddr    - register offset to be read
*        page    - page number to be read
*
* OUTPUTS:
*        data    - value of the read register
*
* RETURNS:
*       GT_OK               - if read successed
*       GT_FAIL               - if read failed
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gprtGetPagedPhyReg
(
    IN  GT_QD_DEV *dev,
    IN  GT_U32  port,
    IN    GT_U32  regAddr,
    IN    GT_U32  page,
    OUT GT_U16* data
);

/*******************************************************************************
* gprtSetPagedPhyReg
*
* DESCRIPTION:
*       This routine writes a value to phy register of the given page
*
* INPUTS:
*        port     - port to be read
*        regAddr    - register offset to be read
*        page    - page number to be read
*        data    - value of the read register
*
* OUTPUTS:
*        None
*
* RETURNS:
*       GT_OK               - if read successed
*       GT_FAIL               - if read failed
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gprtSetPagedPhyReg
(
    IN  GT_QD_DEV *dev,
    IN  GT_U32 port,
    IN    GT_U32 regAddr,
    IN    GT_U32 page,
    IN  GT_U16 data
);


/* gtPortCtrl.c */

/*******************************************************************************
* gprtSetUseCoreTag
*
* DESCRIPTION:
*       This routine set the UseCoreTag bit in Port Control Register.
*            When this bit is cleared to a zero, ingressing frames are considered
*            Tagged if the 16-bits following the frame's Source Address is 0x8100.
*            When this bit is set to a one, ingressing frames are considered Tagged
*            if the 16-bits following the frame's Source Address is equal to the 
*            CoreTag register value.
*
* INPUTS:
*       port  - the logical port number.
*       force - GT_TRUE for force flow control  or GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetUseCoreTag
(
    IN GT_QD_DEV  *dev,
    IN GT_LPORT   port,
    IN GT_BOOL    force
);

/*******************************************************************************
* gprtGetUseCoreTag
*
* DESCRIPTION:
*       This routine get the Use Core Tag state.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       force - GT_TRUE for using core tag register  or GT_FALSE otherwise
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetUseCoreTag
(
    IN  GT_QD_DEV  *dev,
    IN  GT_LPORT   port,
    OUT GT_BOOL    *force
);

/*******************************************************************************
* gprtSetDiscardTagged
*
* DESCRIPTION:
*        When this bit is set to a one, all non-MGMT frames that are processed as 
*        Tagged will be discarded as they enter this switch port. Priority only 
*        tagged frames (with a VID of 0x000) are considered tagged.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE to discard tagged frame, GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetDiscardTagged
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL        mode
);

/*******************************************************************************
* gprtGetDiscardTagged
*
* DESCRIPTION:
*        This routine gets DiscardTagged bit for the given port
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode  - GT_TRUE if DiscardTagged bit is set, GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetDiscardTagged
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL     *mode
);

/*******************************************************************************
* gprtSetDiscardUntagged
*
* DESCRIPTION:
*        When this bit is set to a one, all non-MGMT frames that are processed as 
*        Untagged will be discarded as they enter this switch port. Priority only 
*        tagged frames (with a VID of 0x000) are considered tagged.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE to discard untagged frame, GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetDiscardUntagged
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL        mode
);

/*******************************************************************************
* gprtGetDiscardUntagged
*
* DESCRIPTION:
*        This routine gets DiscardUntagged bit for the given port
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode  - GT_TRUE if DiscardUntagged bit is set, GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetDiscardUntagged
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL     *mode
);

/*******************************************************************************
* gprtSetMapDA
*
* DESCRIPTION:
*        When this bit is set to a one, normal switch operation will occur where a 
*        frame's DA address is used to direct the frame out the correct port.
*        When this be is cleared to a zero, the frame will be sent out the port(s) 
*        defined by ForwardUnknown bits or the DefaultForward bits even if the DA 
*        is ound in the address database.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE to use MapDA, GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetMapDA
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL        mode
);

/*******************************************************************************
* gprtGetMapDA
*
* DESCRIPTION:
*        This routine gets MapDA bit for the given port
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode  - GT_TRUE if MapDA bit is set, GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetMapDA
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL     *mode
);

/*******************************************************************************
* gprtSetDefaultForward
*
* DESCRIPTION:
*        When this bit is set to a one, normal switch operation will occurs and 
*        multicast frames with unknown DA addresses are allowed to egress out this 
*        port (assuming the VLAN settings allow the frame to egress this port too).
*        When this be is cleared to a zero, multicast frames with unknown DA 
*        addresses will not egress out this port.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE to use DefaultForward, GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetDefaultForward
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL        mode
);

/*******************************************************************************
* gprtGetDefaultForward
*
* DESCRIPTION:
*        This routine gets DefaultForward bit for the given port
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode  - GT_TRUE if DefaultForward bit is set, GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetDefaultForward
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL     *mode
);

/*******************************************************************************
* gprtSetEgressMonitorSource
*
* DESCRIPTION:
*        When this be is cleared to a zero, normal network switching occurs.
*        When this bit is set to a one, any frame that egresses out this port will
*        also be sent to the EgressMonitorDest Port
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE to set EgressMonitorSource, GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetEgressMonitorSource
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL        mode
);

/*******************************************************************************
* gprtGetEgressMonitorSource
*
* DESCRIPTION:
*        This routine gets EgressMonitorSource bit for the given port
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode  - GT_TRUE if EgressMonitorSource bit is set, GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetEgressMonitorSource
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL     *mode
);

/*******************************************************************************
* gprtSetIngressMonitorSource
*
* DESCRIPTION:
*        When this be is cleared to a zero, normal network switching occurs.
*        When this bit is set to a one, any frame that egresses out this port will
*        also be sent to the EgressMonitorDest Port
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE to set EgressMonitorSource, GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetIngressMonitorSource
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL        mode
);

/*******************************************************************************
* gprtGetIngressMonitorSource
*
* DESCRIPTION:
*        This routine gets IngressMonitorSource bit for the given port
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode  - GT_TRUE if IngressMonitorSource bit is set, GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetIngressMonitorSource
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL     *mode
);


/* gtPortPAV.c */

/* gtPortRateCtrl.c */

/* gtPortRmon.c */

/*******************************************************************************
* gstatsGetPortCounter3
*
* DESCRIPTION:
*        This routine gets a specific counter of the given port
*
* INPUTS:
*        port - the logical port number.
*        counter - the counter which will be read
*
* OUTPUTS:
*        statsData - points to 32bit data storage for the MIB counter
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*
* COMMENTS:
*        This function supports Gigabit Switch only
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gstatsGetPortCounter3
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT        port,
    IN  GT_STATS_COUNTERS3    counter,
    OUT GT_U32            *statsData
);

/*******************************************************************************
* gstatsGetPortAllCounters3
*
* DESCRIPTION:
*        This routine gets all counters of the given port
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        statsCounterSet - points to GT_STATS_COUNTER_SET for the MIB counters
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*
* COMMENTS:
*        This function supports Gigabit Switch only
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gstatsGetPortAllCounters3
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT        port,
    OUT GT_STATS_COUNTER_SET3    *statsCounterSet
);

/*******************************************************************************
* gstatsGetPortCounter4
*
* DESCRIPTION:
*        This routine gets a specific counter of the given port
*
* INPUTS:
*        port - the logical port number.
*        counter - the counter which will be read
*
* OUTPUTS:
*        statsData - points to 32bit data storage for the MIB counter
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*
* COMMENTS:
*        This function supports Gigabit Switch and Spinnaker family
*
*******************************************************************************/
GT_STATUS gstatsGetPortCounter4
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT        port,
    IN  GT_STATS_COUNTERS4    counter,
    OUT GT_U32            *statsData
);

/*******************************************************************************
* gstatsGetPortAllCounters4
*
* DESCRIPTION:
*        This routine gets all counters of the given port
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        statsCounterSet - points to GT_STATS_COUNTER_SET for the MIB counters
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*
* COMMENTS:
*        This function supports Gigabit Switch and Spinnaker family
*
*******************************************************************************/
GT_STATUS gstatsGetPortAllCounters4
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT        port,
    OUT GT_STATS_COUNTER_SET4    *statsCounterSet
);

/* gtPortStat.c */

/*******************************************************************************
* gprtGetPortCtr2
*
* DESCRIPTION:
*       This routine gets the port InDiscards, InFiltered, and OutFiltered counters.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       ctr - the counters value.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetPortCtr2
(
    IN  GT_QD_DEV       *dev,
    IN  GT_LPORT        port,
    OUT GT_PORT_STAT2   *ctr
);

/* gtPortStatus.c */

/*******************************************************************************
* gprtGetMGMII
*
* DESCRIPTION:
*        SERDES Interface mode. When this bit is cleared to a zero and a PHY is 
*        detected connected to this port, the SERDES interface between this port
*        and the PHY will be SGMII.  When this bit is set toa one and a PHY is
*        detected connected to this port, the SERDES interface between this port 
*        and the PHY will be MGMII. When no PHY is detected on this port and the 
*        SERDES interface is being used, it will be configured in 1000Base-X mode.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE or GT_FALSE
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetMGMII
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
);

/*******************************************************************************
* gprtSetMGMII
*
* DESCRIPTION:
*        SERDES Interface mode. When this bit is cleared to a zero and a PHY is 
*        detected connected to this port, the SERDES interface between this port
*        and the PHY will be SGMII.  When this bit is set toa one and a PHY is
*        detected connected to this port, the SERDES interface between this port 
*        and the PHY will be MGMII. When no PHY is detected on this port and the 
*        SERDES interface is being used, it will be configured in 1000Base-X mode.
*
* INPUTS:
*        port - the logical port number.
*        state - GT_TRUE or GT_FALSE
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetMGMII
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_BOOL      state
);


/* gtQosMap.c */

/* gtSysCtrl.c */

/*******************************************************************************
* gsysSetCoreTagType
*
* DESCRIPTION:
*        This routine sets Ether Core Tag Type.
*        This Ether Type is added to frames that egress the switch as Double Tagged 
*        frames. It is also the Ether Type expected during Ingress to determine if 
*        a frame is Tagged or not on ports configured as UseCoreTag mode.
*
* INPUTS:
*        etherType - Core Tag Type (2 bytes)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysSetCoreTagType
(
    IN GT_QD_DEV    *dev,
    IN GT_U16          etherType
);

/*******************************************************************************
* gsysGetCoreTagType
*
* DESCRIPTION:
*        This routine gets CoreTagType
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        etherType - Core Tag Type (2 bytes)
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetCoreTagType
(
    IN  GT_QD_DEV    *dev,
    OUT GT_U16      *etherType
);

/* gtPortCtrl.c */
/*******************************************************************************
* gsysSetIngressMonitorDest
*
* DESCRIPTION:
*        This routine sets Ingress Monitor Destination Port. Frames that are 
*        targeted toward an Ingress Monitor Destination go out the port number 
*        indicated in these bits. This includes frames received on a Marvell Tag port
*        with the Ingress Monitor type, and frames received on a Network port that 
*        is enabled to be the Ingress Monitor Source Port.
*        If the Ingress Monitor Destination Port resides in this device these bits 
*        should point to the Network port where these frames are to egress. If the 
*        Ingress Monitor Destination Port resides in another device these bits 
*        should point to the Marvell Tag port in this device that is used to get 
*        to the device that contains the Ingress Monitor Destination Port.
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysSetIngressMonitorDest
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port
);

/*******************************************************************************
* gsysGetIngressMonitorDest
*
* DESCRIPTION:
*        This routine gets Ingress Monitor Destination Port.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        port  - the logical port number.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetIngressMonitorDest
(
    IN  GT_QD_DEV    *dev,
    OUT GT_LPORT      *port
);

/*******************************************************************************
* gsysSetEgressMonitorDest
*
* DESCRIPTION:
*        This routine sets Egress Monitor Destination Port. Frames that are 
*        targeted toward an Egress Monitor Destination go out the port number 
*        indicated in these bits. This includes frames received on a Marvell Tag port
*        with the Egress Monitor type, and frames transmitted on a Network port that 
*        is enabled to be the Egress Monitor Source Port.
*        If the Egress Monitor Destination Port resides in this device these bits 
*        should point to the Network port where these frames are to egress. If the 
*        Egress Monitor Destination Port resides in another device these bits 
*        should point to the Marvell Tag port in this device that is used to get 
*        to the device that contains the Egress Monitor Destination Port.
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysSetEgressMonitorDest
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port
);

/*******************************************************************************
* gsysGetEgressMonitorDest
*
* DESCRIPTION:
*        This routine gets Egress Monitor Destination Port.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        port  - the logical port number.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetEgressMonitorDest
(
    IN  GT_QD_DEV    *dev,
    OUT GT_LPORT      *port
);


/* gtSysConfig.c */
/*******************************************************************************
* gsysSetARPDest
*
* DESCRIPTION:
*        This routine sets ARP Monitor Destination Port. Tagged or untagged 
*        frames that ingress Network ports with the Broadcast Destination Address 
*        and with an Ethertype of 0x0806 are mirrored to this port. The ARPDest 
*        should point to the port that directs these frames to the switch's CPU 
*        that will process ARPs. This target port should be a Marvell Tag port so 
*        that frames will egress with a To_CPU Marvell Tag with a CPU Code of ARP.
*        To_CPU Marvell Tag frames with a CPU Code off ARP that ingress a Marvell 
*        Tag port will be sent to the port number definded in ARPDest.
*
*        If ARPDest =  0xF, ARP Monitoring is disabled and ingressing To_CPU ARP 
*        frames will be discarded.
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysSetARPDest
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port
);

/*******************************************************************************
* gsysGetARPDest
*
* DESCRIPTION:
*        This routine gets ARP Monitor Destination Port. Tagged or untagged 
*        frames that ingress Network ports with the Broadcast Destination Address 
*        and with an Ethertype of 0x0806 are mirrored to this port. The ARPDest 
*        should point to the port that directs these frames to the switch's CPU 
*        that will process ARPs. This target port should be a Marvell Tag port so 
*        that frames will egress with a To_CPU Marvell Tag with a CPU Code of ARP.
*        To_CPU Marvell Tag frames with a CPU Code off ARP that ingress a Marvell 
*        Tag port will be sent to the port number definded in ARPDest.
*
*        If ARPDest =  0xF, ARP Monitoring is disabled and ingressing To_CPU ARP 
*        frames will be discarded.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        port  - the logical port number.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetARPDest
(
    IN  GT_QD_DEV    *dev,
    OUT GT_LPORT      *port
);


/* gtPortCtrl.c */

/*******************************************************************************
* gprtSetMessagePort
*
* DESCRIPTION:
*        When the Learn2All bit is set to one, learning message frames are 
*        generated. These frames will be sent out all ports whose Message Port is 
*        set to one.
*         If this feature is used, it is recommended that all Marvell Tag ports, 
*        except for the CPU's port, have their MessagePort bit set to one. 
*        Ports that are not Marvell Tag ports should not have their Message Port
*        bit set to one.
*        
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE to make this port a Message Port. GT_FALSE, otherwise.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtSetMessagePort
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL        mode
);

/*******************************************************************************
* gprtGetMessagePort
*
* DESCRIPTION:
*        When the Learn2All bit is set to one, learning message frames are 
*        generated. These frames will be sent out all ports whose Message Port is 
*        set to one.
*         If this feature is used, it is recommended that all Marvell Tag ports, 
*        except for the CPU's port, have their MessagePort bit set to one. 
*        Ports that are not Marvell Tag ports should not have their Message Port
*        bit set to one.
*
*        
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE to make this port a Message Port. GT_FALSE, otherwise.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gprtGetMessagePort
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL     *mode
);


/*******************************************************************************
* gprtSetTrunkPort
*
* DESCRIPTION:
*        This function enables/disables and sets the trunk ID.
*        
* INPUTS:
*        port - the logical port number.
*        en - GT_TRUE to make the port be a member of a trunk with the given trunkId.
*             GT_FALSE, otherwise.
*        trunkId - valid ID is 0 ~ 15. (0 ~ 31 for Opus)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if trunkId is neither valid nor INVALID_TRUNK_ID
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtSetTrunkPort
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL         en,
    IN GT_U32        trunkId
);


/*******************************************************************************
* gprtGetTrunkPort
*
* DESCRIPTION:
*        This function returns trunk state of the port.
*        When trunk is disabled, trunkId field won't have valid value.
*        
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        en - GT_TRUE, if the port is a member of a trunk,
*             GT_FALSE, otherwise.
*        trunkId - 0 ~ 15(31 for Opus), valid only if en is GT_TRUE
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtGetTrunkPort
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    OUT GT_BOOL     *en,
    OUT GT_U32        *trunkId
);

/*******************************************************************************
* gprtGetGlobal2Reg
*
* DESCRIPTION:
*       This routine reads Switch Global 2 Registers.
*
* INPUTS:
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetGlobal2Reg
(
    IN  GT_QD_DEV    *dev,
    IN  GT_U32         regAddr,
    OUT GT_U16         *data
);

/*******************************************************************************
* gprtSetGlobal2Reg
*
* DESCRIPTION:
*       This routine writes Switch Global2 Registers.
*
* INPUTS:
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetGlobal2Reg
(
    IN  GT_QD_DEV        *dev,
    IN  GT_U32            regAddr,
    IN  GT_U16            data
);

/*******************************************************************************
* gprtGetGlobal3Reg
*
* DESCRIPTION:
*       This routine reads Switch Global 3 Registers.
*
* INPUTS:
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetGlobal3Reg
(
    IN  GT_QD_DEV    *dev,
    IN  GT_U32         regAddr,
    OUT GT_U16         *data
);

/*******************************************************************************
* gprtSetGlobal3Reg
*
* DESCRIPTION:
*       This routine writes Switch Global3 Registers.
*
* INPUTS:
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetGlobal3Reg
(
    IN  GT_QD_DEV        *dev,
    IN  GT_U32            regAddr,
    IN  GT_U16            data
);


/*******************************************************************************
* gsysSetRsvd2CpuEnables
*
* DESCRIPTION:
*        Reserved DA Enables. When the function, gsysSetRsvd2Cpu, is called with 
*        en = GT_TRUE, the 16 reserved multicast DA addresses, whose bit in this 
*        enBits(or register) are also set to a one, are treated as MGMT frames. 
*        All the reserved DA's take the form 01:80:C2:00:00:0x. When x = 0x0, 
*        bit 0 of this register is tested. When x = 0x2, bit 2 of this field is 
*        tested and so on.
*        If the tested bit in this register is cleared to a zero, the frame will 
*        be treated as a normal (non-MGMT) frame.
*
* INPUTS:
*        enBits - bit vector of enabled Reserved Multicast.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysSetRsvd2CpuEnables
(
    IN GT_QD_DEV    *dev,
    IN GT_U16        enBits
);

/*******************************************************************************
* gsysGetRsvd2CpuEnables
*
* DESCRIPTION:
*        Reserved DA Enables. When the function, gsysSetRsvd2Cpu, is called with 
*        en = GT_TRUE, the 16 reserved multicast DA addresses, whose bit in this 
*        enBits(or register) are also set to a one, are treated as MGMT frames. 
*        All the reserved DA's take the form 01:80:C2:00:00:0x. When x = 0x0, 
*        bit 0 of this register is tested. When x = 0x2, bit 2 of this field is 
*        tested and so on.
*        If the tested bit in this register is cleared to a zero, the frame will 
*        be treated as a normal (non-MGMT) frame.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        enBits - bit vector of enabled Reserved Multicast.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetRsvd2CpuEnables
(
    IN  GT_QD_DEV    *dev,
    OUT GT_U16      *enBits
);

/*******************************************************************************
* gsysSetRsvd2Cpu
*
* DESCRIPTION:
*        When the Rsvd2Cpu is set to a one(GT_TRUE), frames with a Destination 
*        Address in the range 01:80:C2:00:00:0x, regardless of their VLAN 
*        membership, will be considered MGMT frames and sent to the port's CPU 
*        Port as long as the associated Rsvd2CpuEnable bit (gsysSetRsvd2CpuEnable 
*        function) for the frames's DA is also set to a one.
*
* INPUTS:
*        en - GT_TRUE if Rsvd2Cpu is set. GT_FALSE, otherwise.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysSetRsvd2Cpu
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL        en
);

/*******************************************************************************
* gsysGetRsvd2Cpu
*
* DESCRIPTION:
*        When the Rsvd2Cpu is set to a one(GT_TRUE), frames with a Destination 
*        Address in the range 01:80:C2:00:00:0x, regardless of their VLAN 
*        membership, will be considered MGMT frames and sent to the port's CPU 
*        Port as long as the associated Rsvd2CpuEnable bit (gsysSetRsvd2CpuEnable 
*        function) for the frames's DA is also set to a one.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        en - GT_TRUE if Rsvd2Cpu is set. GT_FALSE, otherwise.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetRsvd2Cpu
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL      *en
);

/*******************************************************************************
* gsysSetMGMTPri
*
* DESCRIPTION:
*        These bits are used as the PRI[2:0] bits on Rsvd2CPU MGMT frames.
*
* INPUTS:
*        pri - PRI[2:0] bits (should be less than 8)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - If pri is not less than 8.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetMGMTPri
(
    IN GT_QD_DEV    *dev,
    IN GT_U16        pri
);

/*******************************************************************************
* gsysGetMGMTPri
*
* DESCRIPTION:
*        These bits are used as the PRI[2:0] bits on Rsvd2CPU MGMT frames.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        pri - PRI[2:0] bits (should be less than 8)
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetMGMTPri
(
    IN  GT_QD_DEV    *dev,
    OUT GT_U16      *pri
);

/*******************************************************************************
* gsysSetUseDoubleTagData
*
* DESCRIPTION:
*        This bit is used to determine if Double Tag data that is removed from a 
*        Double Tag frame is used or ignored when making switching decisions on 
*        the frame.
*
* INPUTS:
*        en - GT_TRUE to use removed tag data, GT_FALSE otherwise.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetUseDoubleTagData
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL        en
);

/*******************************************************************************
* gsysGetUseDoubleTagData
*
* DESCRIPTION:
*        This bit is used to determine if Double Tag data that is removed from a 
*        Double Tag frame is used or ignored when making switching decisions on 
*        the frame.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        en - GT_TRUE if removed tag data is used, GT_FALSE otherwise.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetUseDoubleTagData
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL      *en
);

/*******************************************************************************
* gsysSetPreventLoops
*
* DESCRIPTION:
*        When a Marvell Tag port receives a Forward Marvell Tag whose Src_Dev 
*        field equals this device's Device Number, the following action will be 
*        taken depending upon the value of this bit.
*        GT_TRUE (1) - The frame will be discarded.
*        GT_FALSE(0) - The frame will be prevented from going out its original 
*                        source port as defined by the frame's Src_Port field.
*
* INPUTS:
*        en - GT_TRUE to discard the frame as described above, GT_FALSE otherwise.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetPreventLoops
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL        en
);

/*******************************************************************************
* gsysGetPreventLoops
*
* DESCRIPTION:
*        When a Marvell Tag port receives a Forward Marvell Tag whose Src_Dev 
*        field equals this device's Device Number, the following action will be 
*        taken depending upon the value of this bit.
*        GT_TRUE (1) - The frame will be discarded.
*        GT_FALSE(0) - The frame will be prevented from going out its original 
*                        source port as defined by the frame's Src_Port field.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        en - GT_TRUE to discard the frame as described above, GT_FALSE otherwise.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetPreventLoops
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL      *en
);

/*******************************************************************************
* gsysSetFlowControlMessage
*
* DESCRIPTION:
*        When this bit is set to one, Marvell Tag Flow Control messages will be 
*        generated when an output queue becomes congested and received Marvell Tag 
*        Flow Control messages will pause MACs inside this device. When this bit 
*        is cleared to a zero, Marvell Tag Flow Control messages will not be 
*        generated and any received will be ignored at the target MAC.
*
* INPUTS:
*        en - GT_TRUE to use Marvell Tag Flow Control message, GT_FALSE otherwise.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetFlowControlMessage
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL        en
);

/*******************************************************************************
* gsysGetFlowControlMessage
*
* DESCRIPTION:
*        When this bit is set to one, Marvell Tag Flow Control messages will be 
*        generated when an output queue becomes congested and received Marvell Tag 
*        Flow Control messages will pause MACs inside this device. When this bit 
*        is cleared to a zero, Marvell Tag Flow Control messages will not be 
*        generated and any received will be ignored at the target MAC.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        en - GT_TRUE to use Marvell Tag Flow Control message, GT_FALSE otherwise.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetFlowControlMessage
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL      *en
);

/*******************************************************************************
* gsysSetForceFlowControlPri
*
* DESCRIPTION:
*        When this bit is set to a one the PRI[2:0] bits of generated Marvell Tag 
*        Flow Control frames will be set to the value of the FC Pri bits (set by 
*        gsysSetFCPri function call). When this bit is cleared to a zero, generated 
*        Marvell Tag Flow Control frames will retain the PRI[2:0] bits from the 
*        frames that caused the congestion. This bit will have no effect if the 
*        FlowControlMessage bit(gsysSetFlowControlMessage function call) is 
*        cleared to a zero.
*
* INPUTS:
*        en - GT_TRUE to use defined PRI bits, GT_FALSE otherwise.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetForceFlowControlPri
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL        en
);

/*******************************************************************************
* gsysGetForceFlowControlPri
*
* DESCRIPTION:
*        When this bit is set to a one the PRI[2:0] bits of generated Marvell Tag 
*        Flow Control frames will be set to the value of the FC Pri bits (set by 
*        gsysSetFCPri function call). When this bit is cleared to a zero, generated 
*        Marvell Tag Flow Control frames will retain the PRI[2:0] bits from the 
*        frames that caused the congestion. This bit will have no effect if the 
*        FlowControlMessage bit(gsysSetFlowControlMessage function call) is 
*        cleared to a zero.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        en - GT_TRUE to use defined PRI bits, GT_FALSE otherwise.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetForceFlowControlPri
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL      *en
);

/*******************************************************************************
* gsysSetFCPri
*
* DESCRIPTION:
*        These bits are used as the PRI[2:0] bits on generated Marvell Tag Flow 
*        Control frames if the ForceFlowControlPri bit(gsysSetForceFlowControlPri)
*        is set to a one.
*
* INPUTS:
*        pri - PRI[2:0] bits (should be less than 8)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - If pri is not less than 8.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetFCPri
(
    IN GT_QD_DEV    *dev,
    IN GT_U16        pri
);

/*******************************************************************************
* gsysGetFCPri
*
* DESCRIPTION:
*        These bits are used as the PRI[2:0] bits on generated Marvell Tag Flow 
*        Control frames if the ForceFlowControlPri bit(gsysSetForceFlowControlPri)
*        is set to a one.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        pri - PRI[2:0] bits (should be less than 8)
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetFCPri
(
    IN  GT_QD_DEV    *dev,
    OUT GT_U16      *pri
);

/*******************************************************************************
* gsysSetFlowCtrlDelay
*
* DESCRIPTION:
*        This function sets Flow control delay time for 10Mbps, 100Mbps, and 
*        1000Mbps. 
*
* INPUTS:
*        sp - PORT_SPEED_10_MBPS, PORT_SPEED_100_MBPS, or PORT_SPEED_1000_MBPS
*        delayTime - actual delay time will be (this value x 2.048uS).
*                    the value cannot exceed 0x1FFF (or 8191 in decimal).
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if sp is not valid or delayTime is > 0x1FFF.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysSetFlowCtrlDelay
(
    IN GT_QD_DEV            *dev,
    IN GT_PORT_SPEED_MODE    sp,
    IN GT_U32                delayTime
);

/*******************************************************************************
* gsysGetFlowCtrlDelay
*
* DESCRIPTION:
*        This function retrieves Flow control delay time for 10Mbps, 100Mbps, and
*        1000Mbps. 
*
* INPUTS:
*        sp - PORT_SPEED_10_MBPS, PORT_SPEED_100_MBPS, or PORT_SPEED_1000_MBPS
*
* OUTPUTS:
*        delayTime - actual delay time will be (this value x 2.048uS).
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if sp is not valid or delayTime is > 0x1FFF.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetFlowCtrlDelay
(
    IN  GT_QD_DEV    *dev,
    IN  GT_PORT_SPEED_MODE    sp,
    OUT GT_U32        *delayTime
);

/*******************************************************************************
* gsysSetDevRoutingTable
*
* DESCRIPTION:
*        This function sets Device to Port mapping (which device is connected to 
*        which port of this device). 
*
* INPUTS:
*        devNum - target device number.
*        portNum - the logical port number.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if devNum >= 32 or port >= total number of ports.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysSetDevRoutingTable
(
    IN GT_QD_DEV    *dev,
    IN GT_U32          devNum,
    IN GT_LPORT     port
);

/*******************************************************************************
* gsysGetDevRoutingTable
*
* DESCRIPTION:
*        This function gets Device to Port mapping (which device is connected to 
*        which port of this device). 
*
* INPUTS:
*        devNum - target device number.
*
* OUTPUTS:
*        portNum - the logical port number.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if devNum >= 32
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetDevRoutingTable
(
    IN  GT_QD_DEV    *dev,
    IN  GT_U32         devNum,
    OUT GT_LPORT     *port
);

/*******************************************************************************
* gsysSetTrunkMaskTable
*
* DESCRIPTION:
*        This function sets Trunk mask vector table for load balancing.
*        This vector will be AND'ed with where the frame was originally egressed to.
*        To insure all trunks are load balanced correctly, the data in this table
*        needs to be correctly configured.
*
* INPUTS:
*        trunkNum - one of the eight Trunk mask vectors.
*        trunkMask - Trunk Mask bits. Bit 0 controls trunk masking for port 0,
*                    bit 1 for port 1 , etc.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if trunkNum > 0x7 or trunMask > 0x7FF (or port vector).
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysSetTrunkMaskTable
(
    IN GT_QD_DEV    *dev,
    IN GT_U32          trunkNum,
    IN GT_U32        trunkMask
);

/*******************************************************************************
* gsysGetTrunkMaskTable
*
* DESCRIPTION:
*        This function sets Trunk mask vector table for load balancing.
*        This vector will be AND'ed with where the frame was originally egressed to.
*        To insure all trunks are load balanced correctly, the data in this table
*        needs to be correctly configured.
*
* INPUTS:
*        trunkNum - one of the eight Trunk mask vectors.
*
* OUTPUTS:
*        trunkMask - Trunk Mask bits. Bit 0 controls trunk masking for port 0,
*                    bit 1 for port 1 , etc.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if trunkNum > 0x7.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetTrunkMaskTable
(
    IN  GT_QD_DEV    *dev,
    IN  GT_U32         trunkNum,
    OUT GT_U32        *trunkMask
);

/*******************************************************************************
* gsysSetHashTrunk
*
* DESCRIPTION:
*        Hash DA & SA for TrunkMask selection. Trunk load balancing is accomplished 
*        by using the frame's DA and SA fields to access one of eight Trunk Masks. 
*        When this bit is set to a one, the hashed computed for address table 
*        lookups is used for the TrunkMask selection. When this bit is cleared to 
*        a zero the lower 3 bits of the frame's DA and SA are XOR'ed together to 
*        select the TrunkMask to use.
*
* INPUTS:
*        en - GT_TRUE to use lookup table, GT_FALSE to use XOR.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetHashTrunk
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL        en
);

/*******************************************************************************
* gsysGetHashTrunk
*
* DESCRIPTION:
*        Hash DA & SA for TrunkMask selection. Trunk load balancing is accomplished 
*        by using the frame's DA and SA fields to access one of eight Trunk Masks. 
*        When this bit is set to a one, the hashed computed for address table 
*        lookups is used for the TrunkMask selection. When this bit is cleared to 
*        a zero the lower 3 bits of the frame's DA and SA are XOR'ed together to 
*        select the TrunkMask to use.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        en - GT_TRUE to use lookup table, GT_FALSE to use XOR.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetHashTrunk
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL      *en
);

/*******************************************************************************
* gsysSetTrunkRouting
*
* DESCRIPTION:
*        This function sets routing information for the given Trunk ID.
*
* INPUTS:
*        trunkId - Trunk ID.
*        trunkRoute - Trunk route bits. Bit 0 controls trunk routing for port 0,
*                    bit 1 for port 1 , etc.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if trunkId > 0xF(0x1F for Opus) or trunkRoute > 0x7FF(or port vector).
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysSetTrunkRouting
(
    IN GT_QD_DEV    *dev,
    IN GT_U32          trunkId,
    IN GT_U32        trunkRoute
);

/*******************************************************************************
* gsysGetTrunkRouting
*
* DESCRIPTION:
*        This function retrieves routing information for the given Trunk ID.
*
* INPUTS:
*        trunkId - Trunk ID.
*
* OUTPUTS:
*        trunkRoute - Trunk route bits. Bit 0 controls trunk routing for port 0,
*                    bit 1 for port 1 , etc.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if trunkId > 0xF(0x1F for Opus).
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetTrunkRouting
(
    IN  GT_QD_DEV    *dev,
    IN  GT_U32         trunkId,
    OUT GT_U32        *trunkRoute
);



/* Prototype added for 88E6095 Rev 1 or Rev 2 */

/* gtPortCtrl.c */
/*******************************************************************************
* gprtGetDiscardBCastMode
*
* DESCRIPTION:
*       This routine gets the Discard Broadcast Mode. If the mode is enabled,
*        all the broadcast frames to the given port will be discarded.
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*        en - GT_TRUE, if enabled,
*             GT_FALSE, otherwise.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetDiscardBCastMode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *en
);

/*******************************************************************************
* gprtSetDiscardBCastMode
*
* DESCRIPTION:
*       This routine sets the Discard Broadcast mode.
*        If the mode is enabled, all the broadcast frames to the given port will 
*        be discarded.
*
* INPUTS:
*       port - logical port number
*        en - GT_TRUE, to enable the mode,
*             GT_FALSE, otherwise.
*
* OUTPUTS:
*        None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetDiscardBCastMode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_BOOL      en
);

/*******************************************************************************
* gprtGetFCOnRateLimitMode
*
* DESCRIPTION:
*       This routine returns mode that tells if ingress rate limiting uses Flow 
*        Control. When this mode is enabled and the port receives frames over the 
*        limit, Ingress Rate Limiting will be performed by stalling the 
*        link partner using flow control, instead of discarding frames.
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*        en - GT_TRUE, if the mode is enabled,
*             GT_FALSE, otherwise.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetFCOnRateLimitMode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *en
);

/*******************************************************************************
* gprtSetFCOnRateLimitMode
*
* DESCRIPTION:
*       This routine sets the mode that tells if ingress rate limiting uses Flow 
*        Control. When this mode is enabled and the port receives frames over the 
*        limit, Ingress Rate Limiting will be performed by stalling the 
*        link partner using flow control, instead of discarding frames.
*
* INPUTS:
*       port - logical port number
*        en - GT_TRUE, to enable the mode,
*             GT_FALSE, otherwise.
*
* OUTPUTS:
*        None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetFCOnRateLimitMode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_BOOL      en
);


/* gtPortRateCtrl.c */

/*******************************************************************************
* grcSetBurstRate
*
* DESCRIPTION:
*       This routine sets the port's ingress data limit based on burst size.
*
* INPUTS:
*       port    - logical port number.
*       bsize    - burst size.
*       rate    - ingress data rate limit. These frames will be discarded after 
*                the ingress rate selected is reached or exceeded. 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_BAD_PARAM        - on bad parameters 
*                                Minimum rate for Burst Size 24K byte is 128Kbps
*                                Minimum rate for Burst Size 48K byte is 256Kbps
*                                Minimum rate for Burst Size 96K byte is 512Kbps
*        GT_NOT_SUPPORTED    - if current device does not support this feature.
*
* COMMENTS: 
*        If the device supports both priority based Rate Limiting and burst size
*        based Rate limiting, user has to manually change the mode to burst size
*        based Rate limiting by calling gsysSetRateLimitMode.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS grcSetBurstRate
(
    IN GT_QD_DEV       *dev,
    IN GT_LPORT        port,
    IN GT_BURST_SIZE   bsize,
    IN GT_BURST_RATE   rate
);

/*******************************************************************************
* grcGetBurstRate
*
* DESCRIPTION:
*       This routine retrieves the port's ingress data limit based on burst size.
*
* INPUTS:
*       port    - logical port number.
*
* OUTPUTS:
*       bsize    - burst size.
*       rate    - ingress data rate limit. These frames will be discarded after 
*                the ingress rate selected is reached or exceeded. 
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS grcGetBurstRate
(
    IN  GT_QD_DEV       *dev,
    IN  GT_LPORT        port,
    OUT GT_BURST_SIZE   *bsize,
    OUT GT_BURST_RATE   *rate
);


/*******************************************************************************
* grcSetTCPBurstRate
*
* DESCRIPTION:
*       This routine sets the port's TCP/IP ingress data limit based on burst size.
*
* INPUTS:
*       port    - logical port number.
*       rate    - ingress data rate limit for TCP/IP packets. These frames will 
*                be discarded after the ingress rate selected is reached or exceeded. 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_BAD_PARAM        - on bad parameters 
*                                Valid rate is GT_BURST_NO_LIMIT, or between
*                                64Kbps and 1500Kbps.
*        GT_NOT_SUPPORTED    - if current device does not support this feature.
*
* COMMENTS: 
*        If the device supports both priority based Rate Limiting and burst size
*        based Rate limiting, user has to manually change the mode to burst size
*        based Rate limiting by calling gsysSetRateLimitMode.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS grcSetTCPBurstRate
(
    IN GT_QD_DEV       *dev,
    IN GT_LPORT        port,
    IN GT_BURST_RATE   rate
);


/*******************************************************************************
* grcGetTCPBurstRate
*
* DESCRIPTION:
*       This routine sets the port's TCP/IP ingress data limit based on burst size.
*
* INPUTS:
*       port    - logical port number.
*
* OUTPUTS:
*       rate    - ingress data rate limit for TCP/IP packets. These frames will 
*                be discarded after the ingress rate selected is reached or exceeded. 
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_BAD_VALUE        - register value is not known
*        GT_NOT_SUPPORTED    - if current device does not support this feature.
*
* COMMENTS: 
*        If the device supports both priority based Rate Limiting and burst size
*        based Rate limiting, user has to manually change the mode to burst size
*        based Rate limiting by calling gsysSetRateLimitMode.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS grcGetTCPBurstRate
(
    IN  GT_QD_DEV       *dev,
    IN  GT_LPORT        port,
    OUT GT_BURST_RATE   *rate
);


/* gtSysCtrl.c */
/*******************************************************************************
* gsysSetRateLimitMode
*
* DESCRIPTION:
*        Ingress Rate Limiting can be either Priority based or Burst Size based.
*        This routine sets which mode to use.
*
* INPUTS:
*        mode - either GT_RATE_PRI_BASE or GT_RATE_BURST_BASE
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if invalid mode is used.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetRateLimitMode
(
    IN GT_QD_DEV    *dev,
    IN GT_INGRESS_RATE_MODE mode
);

/*******************************************************************************
* gsysGetRateLimitMode
*
* DESCRIPTION:
*        Ingress Rate Limiting can be either Priority based or Burst Size based.
*        This routine gets which mode is being used.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        mode - either GT_RATE_PRI_BASE or GT_RATE_BURST_BASE
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysGetRateLimitMode
(
    IN  GT_QD_DEV    *dev,
    OUT GT_INGRESS_RATE_MODE *mode
);

/*******************************************************************************
* gsysSetAgeInt
*
* DESCRIPTION:
*        Enable/Disable Age Refresh Interrupt. If CPU Directed Learning is being
*        used (gprtSetLockedPort), it may be desirable to know when an address is
*        still being used before it totally ages out of the switch. This can be 
*        accomplished by enabling Age Refresh Interrupt (or ATU Age Violation Int).
*        An ATU Age Violation looks identical to and reported the same as an ATU 
*        Miss Violation. The only difference is when this reported. Normal ATU Miss
*        Violation only occur if a new SA arrives at a LockedPort. The Age version 
*        of the ATU Miss Violation occurs if an SA arrives at a LockedPort, where
*        the address is contained in the ATU's database, but where its EntryState 
*        is less than 0x4 (i.e., it has aged more than 1/2 way).
*        GT_ATU_PROB Interrupt should be enabled for this interrupt to occur.
*        Refer to eventSetActive routine to enable GT_ATU_PROB.
*        
*
* INPUTS:
*        en - GT_TRUE, to enable,
*             GT_FALSE, otherwise.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetAgeInt
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL        en
);

/*******************************************************************************
* gsysGetAgeInt
*
* DESCRIPTION:
*        Get state of Age Refresh Interrupt mode. If CPU Directed Learning is being
*        used (gprtSetLockedPort), it may be desirable to know when an address is
*        still being used before it totally ages out of the switch. This can be 
*        accomplished by enabling Age Refresh Interrupt (or ATU Age Violation Int).
*        An ATU Age Violation looks identical to and reported the same as an ATU 
*        Miss Violation. The only difference is when this reported. Normal ATU Miss
*        Violation only occur if a new SA arrives at a LockedPort. The Age version 
*        of the ATU Miss Violation occurs if an SA arrives at a LockedPort, where
*        the address is contained in the ATU's database, but where its EntryState 
*        is less than 0x4 (i.e., it has aged more than 1/2 way).
*        GT_ATU_PROB Interrupt should be enabled for this interrupt to occur.
*        Refer to eventSetActive routine to enable GT_ATU_PROB.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        en - GT_TRUE, if enabled,
*             GT_FALSE, otherwise.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysGetAgeInt
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL        *en
);


/* For Zephyr */

/* gtPhyCtrl.c */
/*******************************************************************************
* gprtGetPhyLinkStatus
*
* DESCRIPTION:
*       This routine retrieves the Link status.
*
* INPUTS:
*         port     - The logical port number
*
* OUTPUTS:
*       linkStatus - GT_FALSE if link is not established,
*                     GT_TRUE if link is established.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*        
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gprtGetPhyLinkStatus
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT  port,
    IN GT_BOOL      *linkStatus
);


/*******************************************************************************
* gprtSetPktGenEnable
*
* DESCRIPTION:
*       This routine enables or disables Packet Generator.
*       Link should be established first prior to enabling the packet generator,
*       and generator will generate packets at the speed of the established link.
*        When enables packet generator, the following information should be 
*       provided:
*           Payload Type:  either Random or 5AA55AA5
*           Packet Length: either 64 or 1514 bytes
*           Error Packet:  either Error packet or normal packet
*
* INPUTS:
*         port     - The logical port number
*       en      - GT_TRUE to enable, GT_FALSE to disable
*       pktInfo - packet information(GT_PG structure pointer), if en is GT_TRUE.
*                 ignored, if en is GT_FALSE
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*        
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gprtSetPktGenEnable
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT  port,
    IN GT_BOOL   en,
    IN GT_PG     *pktInfo
);



/* gtSysCtrl.c */

/*******************************************************************************
* gsysSetForceSnoopPri
*
* DESCRIPTION:
*        Force Snooping Priority. The priority on IGMP or MLD Snoop frames are
*        set to the SnoopPri value (gsysSetSnoopPri API) when Force Snooping
*       Priority is enabled. When it's disabled, the priority on these frames
*        is not modified.
*
* INPUTS:
*        en - GT_TRUE to use defined PRI bits, GT_FALSE otherwise.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetForceSnoopPri
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL        en
);

/*******************************************************************************
* gsysGetForceSnoopPri
*
* DESCRIPTION:
*        Force Snooping Priority. The priority on IGMP or MLD Snoop frames are
*        set to the SnoopPri value (gsysSetSnoopPri API) when Force Snooping
*       Priority is enabled. When it's disabled, the priority on these frames
*        is not modified.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        en - GT_TRUE to use defined PRI bits, GT_FALSE otherwise.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetForceSnoopPri
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL      *en
);


/*******************************************************************************
* gsysSetSnoopPri
*
* DESCRIPTION:
*        Snoop Priority. When ForceSnoopPri (gsysSetForceSnoopPri API) is enabled,
*       this priority is used as the egressing frame's PRI[2:0] bits on generated
*       Marvell Tag To_CPU Snoop frames and higher 2 bits of the priority are
*       used as the internal Queue Priority to use on IGMP/MLD snoop frames.
*
* INPUTS:
*        pri - PRI[2:0] bits (should be less than 8)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - If pri is not less than 8.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetSnoopPri
(
    IN GT_QD_DEV    *dev,
    IN GT_U16        pri
);


/*******************************************************************************
* gsysGetSnoopPri
*
* DESCRIPTION:
*        Snoop Priority. When ForceSnoopPri (gsysSetForceSnoopPri API) is enabled,
*       this priority is used as the egressing frame's PRI[2:0] bits on generated
*       Marvell Tag To_CPU Snoop frames and higher 2 bits of the priority are
*       used as the internal Queue Priority to use on IGMP/MLD snoop frames.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        pri - PRI[2:0] bits (should be less than 8)
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetSnoopPri
(
    IN  GT_QD_DEV    *dev,
    OUT GT_U16      *pri
);


/*******************************************************************************
* gsysSetForceARPPri
*
* DESCRIPTION:
*        Force ARP Priority. The priority on ARP frames are set to the ARPPri 
*       value (gsysSetARPPri API) when Force ARP Priority is enabled. When it's 
*       disabled, the priority on these frames is not modified.
*
* INPUTS:
*        en - GT_TRUE to use defined PRI bits, GT_FALSE otherwise.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetForceARPPri
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL        en
);

/*******************************************************************************
* gsysGetForceARPPri
*
* DESCRIPTION:
*        Force ARP Priority. The priority on ARP frames are set to the ARPPri 
*       value (gsysSetARPPri API) when Force ARP Priority is enabled. When it's 
*       disabled, the priority on these frames is not modified.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        en - GT_TRUE to use defined PRI bits, GT_FALSE otherwise.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetForceARPPri
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL      *en
);


/*******************************************************************************
* gsysSetARPPri
*
* DESCRIPTION:
*        ARP Priority. When ForceARPPri (gsysSetForceARPPri API) is enabled,
*       this priority is used as the egressing frame's PRI[2:0] bits on generated
*       Marvell Tag To_CPU ARP frames and higher 2 bits of the priority are
*       used as the internal Queue Priority to use on ARP frames.
*
* INPUTS:
*        pri - PRI[2:0] bits (should be less than 8)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - If pri is not less than 8.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetARPPri
(
    IN GT_QD_DEV    *dev,
    IN GT_U16        pri
);


/*******************************************************************************
* gsysGetARPPri
*
* DESCRIPTION:
*        ARP Priority. When ForceARPPri (gsysSetForceARPPri API) is enabled,
*       this priority is used as the egressing frame's PRI[2:0] bits on generated
*       Marvell Tag To_CPU ARP frames and higher 2 bits of the priority are
*       used as the internal Queue Priority to use on ARP frames.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        pri - PRI[2:0] bits (should be less than 8)
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetARPPri
(
    IN  GT_QD_DEV    *dev,
    OUT GT_U16      *pri
);


/* added for 88E6065 */

/* gtBrgVlan.c */

/********************************************************************
* gvlnSetForceMap
*
* DESCRIPTION:
*       This routine enables/disables Force Map feature.
*        When Force Map feature is enabled, all received frames will be
*        considered MGMT and they are mapped to the port or ports defined
*        in the VLAN Table overriding the mapping from the address database.
*
* INPUTS:
*       port    - logical port number to set.
*       mode    - GT_TRUE, to enable force map feature
*                 GT_FAULSE, otherwise 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gvlnSetForceMap
(
    IN  GT_QD_DEV   *dev,
    IN  GT_LPORT     port,
    IN  GT_BOOL      mode
);


/********************************************************************
* gvlnGetForceMap
*
* DESCRIPTION:
*       This routine checks if Force Map feature is enabled.
*        When Force Map feature is enabled, all received frames will be
*        considered MGMT and they are mapped to the port or ports defined
*        in the VLAN Table overriding the mapping from the address database.
*
* INPUTS:
*       port    - logical port number to set.
*
* OUTPUTS:
*       mode    - GT_TRUE, to enable force map feature
*                 GT_FAULSE, otherwise 
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gvlnGetForceMap
(
    IN  GT_QD_DEV   *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *mode
);

/* gtEvents.c */

/*******************************************************************************
* geventSetAgeIntEn
*
* DESCRIPTION:
*        This routine enables/disables Age Interrupt for a port.
*        When it's enabled, ATU Age Violation interrupts from this port are enabled.
*        An Age Violation will occur anytime a port is Locked(gprtSetLockedPort) 
*        and the ingressing frame's SA is contained in the ATU as a non-Static 
*        entry with a EntryState less than 0x4.
*
* INPUTS:
*        port - the logical port number
*        mode - GT_TRUE to enable Age Interrupt,
*               GT_FALUSE to disable
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS geventSetAgeIntEn
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    IN  GT_BOOL        mode
);

/*******************************************************************************
* geventGetAgeIntEn
*
* DESCRIPTION:
*        This routine gets Age Interrupt Enable for the port.
*        When it's enabled, ATU Age Violation interrupts from this port are enabled.
*        An Age Violation will occur anytime a port is Locked(gprtSetLockedPort) 
*        and the ingressing frame's SA is contained in the ATU as a non-Static 
*        entry with a EntryState less than 0x4.
*
* INPUTS:
*        port - the logical port number
*        mode - GT_TRUE to enable Age Interrupt,
*               GT_FALUSE to disable
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS geventGetAgeIntEn
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *mode
);


/* gtPIRL.c */

/*******************************************************************************
* pirlDumpResource
*
* DESCRIPTION:
*       This function dumps IRL Resource register values.
*
* INPUTS:
*        irlUnit  - resource unit to be accessed
*        dataLen  - data size.
*
* OUTPUTS:
*       data - IRL Resource data
*
* RETURNS:
*       GT_OK on success,
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pirlDumpResource
(
    IN  GT_QD_DEV              *dev,
    IN    GT_U32                irlUnit,
    IN    GT_U32                dataLen,
    OUT GT_U16                *data
);



/*******************************************************************************
* gpirlActivate
*
* DESCRIPTION:
*       This routine activates Ingress Rate Limiting for the given ports by 
*        initializing a resource bucket, assigning ports, and configuring
*        Bucket Parameters.
*
* INPUTS:
*        irlUnit  - bucket to be used (0 ~ 11).
*       portVec  - the list of ports that share the bucket.
*        pirlData - PIRL resource parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gpirlActivate
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U32        irlUnit,
    IN  GT_U32        portVec,
    IN  GT_PIRL_DATA    *pirlData
);

/*******************************************************************************
* gpirlDeactivate
*
* DESCRIPTION:
*       This routine deactivates Ingress Rate Limiting for the given bucket.
*        It simply removes every ports from the Ingress Rate Resource.
*        It is assumed that gpirlActivate has been successfully called with
*        the irlUnit before this function is called.
*
* INPUTS:
*        irlUnit  - bucket to be deactivated
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gpirlDeactivate
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U32        irlUnit
);

/*******************************************************************************
* gpirlUpdateParam
*
* DESCRIPTION:
*       This routine updates IRL Parameter.
*        It is assumed that gpirlActivate has been successfully called with
*        the given irlUnit before this function is called.
*
* INPUTS:
*        irlUnit  - bucket to be used (0 ~ 11)
*        pirlData - PIRL resource parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gpirlUpdateParam
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U32        irlUnit,
    IN  GT_PIRL_DATA    *pirlData
);

/*******************************************************************************
* gpirlReadParam
*
* DESCRIPTION:
*       This routine retrieves IRL Parameter.
*        It is assumed that gpirlActivate has been successfully called with
*        the given irlUnit before this function is called.
*
* INPUTS:
*        irlUnit  - bucket to be used (0 ~ 11).
*
* OUTPUTS:
*        pirlData - PIRL resource parameters.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gpirlReadParam
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U32        irlUnit,
    OUT GT_PIRL_DATA    *pirlData
);

/*******************************************************************************
* gpirlUpdatePortVec
*
* DESCRIPTION:
*       This routine updates port list that share the bucket.
*        It is assumed that gpirlActivate has been successfully called with
*        the given irlUnit before this function is called.
*
* INPUTS:
*        irlUnit  - bucket to be used (0 ~ 11).
*       portVec  - the list of ports that share the bucket.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gpirlUpdatePortVec
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U32        irlUnit,
    IN  GT_U32        portVec
);

/*******************************************************************************
* gpirlReadPortVec
*
* DESCRIPTION:
*       This routine retrieves port list that share the bucket.
*        It is assumed that gpirlActivate has been successfully called with
*        the given irlUnit before this function is called.
*
* INPUTS:
*        irlUnit  - bucket to be used (0 ~ 11).
*
* OUTPUTS:
*       portVec  - the list of ports that share the bucket.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gpirlReadPortVec
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U32        irlUnit,
    OUT GT_U32        *portVec
);

/*******************************************************************************
* grcGetPirlFcMode
*
* DESCRIPTION:
*       This routine gets Port Ingress Rate Limit Flow Control mode.
*        When EBSLimitAction is programmed to generate a flow control message, 
*        the deassertion of flow control is controlled by this mode.
*            GT_PIRL_FC_DEASSERT_EMPTY:
*                De-assert when the ingress rate resource has become empty
*            GT_PIRL_FC_DEASSERT_CBS_LIMIT
*                De-assert when the ingress rate resource has enough room as
*                specified by the CBSLimit.
*        Please refer to GT_PIRL_RESOURCE structure for EBSLimitAction and
*        CBSLimit.
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*        mode - GT_PIRL_FC_DEASSERT enum type
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*        GT_NOT_SUPPORTED    - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS grcGetPirlFcMode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_PIRL_FC_DEASSERT        *mode
);

/*******************************************************************************
* gpirlGetIngressRateResource
*
* DESCRIPTION:
*       This routine gets Ingress Rate Limiting Resources assigned to the port.
*        This vector is used to attach specific counter resources to the physical
*        port. And the same counter resource can be attached to more than one port.
*
* INPUTS:
*       port   - logical port number
*
* OUTPUTS:
*        resVec - resource vector (bit 0 for irl unit 0, bit 1 for irl unit 1, etc.)
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*        GT_NOT_SUPPORTED    - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gpirlGetIngressRateResource
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_U32        *resVec
);

/*******************************************************************************
* gpirlSetCurTimeUpInt
*
* DESCRIPTION:
*       This function sets the current time update interval.
*        Please contact FAE for detailed information.
*
* INPUTS:
*       upInt - updata interval (0 ~ 7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gpirlSetCurTimeUpInt
(
    IN  GT_QD_DEV              *dev,
    IN    GT_U32                upInt
);


/* gtPortStatus.c */

/*******************************************************************************
* gprtGetPxMode
*
* DESCRIPTION:
*        This routine retrives 4 bits of Px_MODE Configuration value.
*        If speed and duplex modes are forced, the returned mode value would be
*        different from the configuration pin values.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        mode - Px_MODE configuration value
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetPxMode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_U32      *mode
);

/*******************************************************************************
* gprtGetMiiInterface
*
* DESCRIPTION:
*        This routine retrives Mii Interface Mode.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE if Mii Interface is enabled,
*                  GT_FALSE otherwise.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gprtGetMiiInterface
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
);

/*******************************************************************************
* gprtGetFdFlowDis
*
* DESCRIPTION:
*        This routine retrives the read time value of the Full Duplex Flow Disable.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE if Full Duplex Flow Disable.
*                   GT_FALSE otherwise.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gprtGetFdFlowDis
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
);

/*******************************************************************************
* gprtGetHdFlowDis
*
* DESCRIPTION:
*        This routine retrives the read time value of the Half Duplex Flow Disable.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE if Half Duplex Flow Disable.
*                   GT_FALSE otherwise.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gprtGetHdFlowDis
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
);

/*******************************************************************************
* gprtGetOutQSize
*
* DESCRIPTION:
*        This routine gets egress queue size counter value.
*        This counter reflects the current number of Egress buffers switched to 
*        this port and each queue. 
*        The que=0xf is the total number of buffers across all four priority queues.
*
* INPUTS:
*        port - the logical port number
*        que -  the queue number
*
* OUTPUTS:
*        count - egress queue size counter value
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - set error queue number.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtGetOutQSize
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
	IN  GT_U16       que,
    OUT GT_U16       *count
);


/* gtPortCtrl.c */

/*******************************************************************************
* gprtSetSAFiltering
*
* DESCRIPTION:
*        This routine set the Source Address(SA) fitering method.
*            GT_SA_FILTERING_DISABLE :
*                no frame will be filtered.
*            GT_SA_DROP_ON_LOCK :
*                discard if SA field is not in the ATU's address database.
*            GT_SA_DROP_ON_UNLOC : 
*                discard if SA field is in the ATU's address database as Static 
*                entry with a PortVec of all zeros.
*            GT_SA_DROP_TO_CPU : 
*                Ingressing frames will be mapped to the CPU Port if their SA 
*                field is in the ATU's address database as Static entry with a 
*                PortVec of all zeros. Otherwise, the frames will be discarded 
*                if their SA field is not in the ATU's address database or if this
*                port's bit is not set in the PortVec bits for the frame's SA.
*        
* INPUTS:
*       port - the logical port number.
*       mode - GT_SA_FILTERING structure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS gprtSetSAFiltering
(
    IN GT_QD_DEV  *dev,
    IN GT_LPORT   port,
    IN GT_SA_FILTERING    mode
);

/*******************************************************************************
* gprtGetSAFiltering
*
* DESCRIPTION:
*        This routine gets the Source Address(SA) fitering method.
*            GT_SA_FILTERING_DISABLE :
*                no frame will be filtered.
*            GT_SA_DROP_ON_LOCK :
*                discard if SA field is not in the ATU's address database.
*            GT_SA_DROP_ON_UNLOC : 
*                discard if SA field is in the ATU's address database as Static 
*                entry with a PortVec of all zeros.
*            GT_SA_DROP_TO_CPU : 
*                Ingressing frames will be mapped to the CPU Port if their SA 
*                field is in the ATU's address database as Static entry with a 
*                PortVec of all zeros. Otherwise, the frames will be discarded 
*                if their SA field is not in the ATU's address database or if this
*                port's bit is not set in the PortVec bits for the frame's SA.
*        
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - GT_SA_FILTERING structure
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS gprtGetSAFiltering
(
    IN  GT_QD_DEV  *dev,
    IN  GT_LPORT   port,
    OUT GT_SA_FILTERING    *mode
);


/*******************************************************************************
* gprtSetARPtoCPU
*
* DESCRIPTION:
*        When ARPtoCPU is set to GT_TRUE, ARP frames are mapped to the CPU port.
*        
* INPUTS:
*       port - the logical port number.
*       mode - GT_TRUE, to map ARP frames to CPU Port,
*               GT_FALSE, otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS gprtSetARPtoCPU
(
    IN GT_QD_DEV  *dev,
    IN GT_LPORT   port,
    IN GT_BOOL    mode
);


/*******************************************************************************
* gprtGetARPtoCPU
*
* DESCRIPTION:
*        When ARPtoCPU is set to GT_TRUE, ARP frames are mapped to the CPU port.
*        
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - GT_TRUE, to map ARP frames to CPU Port,
*               GT_FALSE, otherwise.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS gprtGetARPtoCPU
(
    IN  GT_QD_DEV  *dev,
    IN  GT_LPORT   port,
    OUT GT_BOOL    *mode
);

/*******************************************************************************
* gprtSetEgressFlood
*
* DESCRIPTION:
*       This routine set Egress Flooding Mode.
*        Frames with unknown DA (Destination Address that is not in ATU database)
*        generally flood out all the ports. This mode can be used to prevent
*        those frames from egressing this port as follows:
*            GT_BLOCK_EGRESS_UNKNOWN
*                do not egress frame with unknown DA (both unicast and multicast)
*            GT_BLOCK_EGRESS_UNKNOWN_MULTICAST
*                do not egress frame with unknown multicast DA
*            GT_BLOCK_EGRESS_UNKNOWN_UNICAST
*                do not egress frame with unknown unicast DA
*            GT_BLOCK_EGRESS_NONE
*                egress all frames with unknown DA
*
* INPUTS:
*       port - the logical port number.
*       mode - GT_EGRESS_FLOOD structure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtSetEgressFlood
(
    IN  GT_QD_DEV   *dev,
    IN  GT_LPORT     port,
    IN  GT_EGRESS_FLOOD      mode
);

/*******************************************************************************
* gprtGetEgressFlood
*
* DESCRIPTION:
*       This routine gets Egress Flooding Mode.
*        Frames with unknown DA (Destination Address that is not in ATU database)
*        generally flood out all the ports. This mode can be used to prevent
*        those frames from egressing this port as follows:
*            GT_BLOCK_EGRESS_UNKNOWN
*                do not egress frame with unknown DA (both unicast and multicast)
*            GT_BLOCK_EGRESS_UNKNOWN_MULTICAST
*                do not egress frame with unknown multicast DA
*            GT_BLOCK_EGRESS_UNKNOWN_UNICAST
*                do not egress frame with unknown unicast DA
*            GT_BLOCK_EGRESS_NONE
*                egress all frames with unknown DA
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - GT_EGRESS_FLOOD structure
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtGetEgressFlood
(
    IN  GT_QD_DEV   *dev,
    IN  GT_LPORT     port,
    OUT GT_EGRESS_FLOOD      *mode
);

/*******************************************************************************
* gprtSetPortSched
*
* DESCRIPTION:
*        This routine sets Port Scheduling Mode.
*        When usePortSched is enablied, this mode is used to select the Queue
*        controller's scheduling on the port as follows:
*            GT_PORT_SCHED_WEIGHTED_RRB - use 8,4,2,1 weighted fair scheduling
*            GT_PORT_SCHED_STRICT_PRI - use a strict priority scheme
*
* INPUTS:
*        port - the logical port number
*        mode - GT_PORT_SCHED_MODE enum type
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtSetPortSched
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    IN  GT_PORT_SCHED_MODE        mode
);

/*******************************************************************************
* gprtGetPortSched
*
* DESCRIPTION:
*        This routine gets Port Scheduling Mode.
*        When usePortSched is enablied, this mode is used to select the Queue
*        controller's scheduling on the port as follows:
*            GT_PORT_SCHED_WEIGHTED_RRB - use 8,4,2,1 weighted fair scheduling
*            GT_PORT_SCHED_STRICT_PRI - use a strict priority scheme
*
* INPUTS:
*        port - the logical port number
*
* OUTPUTS:
*        mode - GT_PORT_SCHED_MODE enum type
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtGetPortSched
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_PORT_SCHED_MODE        *mode
);


/*******************************************************************************
* gprtSetProviderTag
*
* DESCRIPTION:
*        This routine sets Provider Tag which indicates the provider tag (Ether 
*        Type) value that needs to be matched to in ingress to determine if a
*        frame is Provider tagged or not.
*
* INPUTS:
*        port - the logical port number
*        tag  - Provider Tag (Ether Type)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtSetProviderTag
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    IN  GT_U16        tag
);

/*******************************************************************************
* gprtGetProviderTag
*
* DESCRIPTION:
*        This routine gets Provider Tag which indicates the provider tag (Ether 
*        Type) value that needs to be matched to in ingress to determine if a
*        frame is Provider tagged or not.
*
* INPUTS:
*        port - the logical port number
*
* OUTPUTS:
*        tag  - Provider Tag (Ether Type)
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtGetProviderTag
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_U16        *tag
);



/* gtPortRateCtrl.c */

/*******************************************************************************
* grcSetVidNrlEn
*
* DESCRIPTION:
*       This routine enables/disables VID None Rate Limit (NRL).
*        When VID NRL is enabled and the determined VID of a frame results in a VID
*        whose VIDNonRateLimit in the VTU Table is set to GT_TURE, then the frame
*        will not be ingress nor egress rate limited.
*
* INPUTS:
*       port - logical port number.
*        mode - GT_TRUE to enable VID None Rate Limit
*               GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*        GT_NOT_SUPPORTED    - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS grcSetVidNrlEn
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    IN  GT_BOOL        mode
);

/*******************************************************************************
* grcGetVidNrlEn
*
* DESCRIPTION:
*       This routine gets VID None Rate Limit (NRL) mode.
*        When VID NRL is enabled and the determined VID of a frame results in a VID
*        whose VIDNonRateLimit in the VTU Table is set to GT_TURE, then the frame
*        will not be ingress nor egress rate limited.
*
* INPUTS:
*       port - logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE to enable VID None Rate Limit
*               GT_FALSE otherwise
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*        GT_NOT_SUPPORTED    - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS grcGetVidNrlEn
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *mode
);

/*******************************************************************************
* grcSetSaNrlEn
*
* DESCRIPTION:
*       This routine enables/disables SA None Rate Limit (NRL).
*        When SA NRL is enabled and the source address of a frame results in a ATU
*        hit where the SA's MAC address returns an EntryState that indicates Non
*        Rate Limited, then the frame will not be ingress nor egress rate limited.
*
* INPUTS:
*       port - logical port number.
*        mode - GT_TRUE to enable SA None Rate Limit
*               GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*        GT_NOT_SUPPORTED    - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS grcSetSaNrlEn
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    IN  GT_BOOL        mode
);

/*******************************************************************************
* grcGetSaNrlEn
*
* DESCRIPTION:
*       This routine gets SA None Rate Limit (NRL) mode.
*        When SA NRL is enabled and the source address of a frame results in a ATU
*        hit where the SA's MAC address returns an EntryState that indicates Non
*        Rate Limited, then the frame will not be ingress nor egress rate limited.
*
* INPUTS:
*       port - logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE to enable SA None Rate Limit
*               GT_FALSE otherwise
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*        GT_NOT_SUPPORTED    - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS grcGetSaNrlEn
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *mode
);

/*******************************************************************************
* grcSetDaNrlEn
*
* DESCRIPTION:
*       This routine enables/disables DA None Rate Limit (NRL).
*        When DA NRL is enabled and the destination address of a frame results in 
*        a ATU hit where the DA's MAC address returns an EntryState that indicates 
*        Non Rate Limited, then the frame will not be ingress nor egress rate 
*        limited.
*
* INPUTS:
*       port - logical port number.
*        mode - GT_TRUE to enable DA None Rate Limit
*               GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*        GT_NOT_SUPPORTED    - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS grcSetDaNrlEn
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    IN  GT_BOOL        mode
);

/*******************************************************************************
* grcGetDaNrlEn
*
* DESCRIPTION:
*       This routine gets SA None Rate Limit (NRL) mode.
*        When DA NRL is enabled and the destination address of a frame results in 
*        a ATU hit where the DA's MAC address returns an EntryState that indicates 
*        Non Rate Limited, then the frame will not be ingress nor egress rate 
*        limited.
*
* INPUTS:
*       port - logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE to enable DA None Rate Limit
*               GT_FALSE otherwise
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*        GT_NOT_SUPPORTED    - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS grcGetDaNrlEn
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *mode
);

/*******************************************************************************
* grcSetELimitMode
*
* DESCRIPTION:
*       This routine sets Egress Rate Limit counting mode.
*        The supported modes are as follows:
*            GT_PIRL_ELIMIT_FRAME -
*                Count the number of frames
*            GT_PIRL_ELIMIT_LAYER1 -
*                Count all Layer 1 bytes: 
*                Preamble (8bytes) + Frame's DA to CRC + IFG (12bytes)
*            GT_PIRL_ELIMIT_LAYER2 -
*                Count all Layer 2 bytes: Frame's DA to CRC
*            GT_PIRL_ELIMIT_LAYER1 -
*                Count all Layer 1 bytes: 
*                Frame's DA to CRC - 18 - 4 (if frame is tagged)
*
* INPUTS:
*       port - logical port number
*        mode - GT_PIRL_ELIMIT_MODE enum type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*        GT_NOT_SUPPORTED    - if current device does not support this feature.
*
* COMMENTS: 
*        GT_PIRL_ELIMIT_FRAME mode is supported by only a few devices.
*        Please refer to the device datasheet for details.
*
*******************************************************************************/
GT_STATUS grcSetELimitMode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    IN  GT_PIRL_ELIMIT_MODE        mode
);

/*******************************************************************************
* grcGetELimitMode
*
* DESCRIPTION:
*       This routine gets Egress Rate Limit counting mode.
*        The supported modes are as follows:
*            GT_PIRL_ELIMIT_FRAME -
*                Count the number of frames
*            GT_PIRL_ELIMIT_LAYER1 -
*                Count all Layer 1 bytes: 
*                Preamble (8bytes) + Frame's DA to CRC + IFG (12bytes)
*            GT_PIRL_ELIMIT_LAYER2 -
*                Count all Layer 2 bytes: Frame's DA to CRC
*            GT_PIRL_ELIMIT_LAYER1 -
*                Count all Layer 1 bytes: 
*                Frame's DA to CRC - 18 - 4 (if frame is tagged)
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*        mode - GT_PIRL_ELIMIT_MODE enum type
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*        GT_NOT_SUPPORTED    - if current device does not support this feature.
*
* COMMENTS: 
*        GT_PIRL_ELIMIT_FRAME mode is supported by only a few devices.
*        Please refer to the device datasheet for details.
*
*******************************************************************************/
GT_STATUS grcGetELimitMode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_PIRL_ELIMIT_MODE        *mode
);

/*******************************************************************************
* grcSetRsvdNrlEn
*
* DESCRIPTION:
*       This routine sets Reserved Non Rate Limit.
*        When this feature is enabled, frames that match the requirements of the 
*        Rsvd2Cpu bit below will also be considered to be ingress and egress non 
*        rate limited.
*
* INPUTS:
*       en - GT_TRUE to enable Reserved Non Rate Limit,
*             GT_FALSE to disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS grcSetRsvdNrlEn
(
    IN  GT_QD_DEV *dev,
    IN  GT_BOOL   en
);

/*******************************************************************************
* grcGetRsvdNrlEn
*
* DESCRIPTION:
*       This routine gets Reserved Non Rate Limit.
*        When this feature is enabled, frames that match the requirements of the 
*        Rsvd2Cpu bit below will also be considered to be ingress and egress non 
*        rate limited.
*
* INPUTS:
*       en - GT_TRUE to enable Reserved Non Rate Limit,
*             GT_FALSE to disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS grcGetRsvdNrlEn
(
    IN  GT_QD_DEV *dev,
    OUT GT_BOOL   *en
);


/* gtPortRmon.c */

/*******************************************************************************
* gstatsGetRealtimePortCounter
*
* DESCRIPTION:
*        This routine gets a specific realtime counter of the given port
*
* INPUTS:
*        port - the logical port number.
*        counter - the counter which will be read
*
* OUTPUTS:
*        statsData - points to 32bit data storage for the MIB counter
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gstatsGetRealtimePortCounter
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT        port,
    IN  GT_STATS_COUNTERS3    counter,
    OUT GT_U32            *statsData
);


/* gtQosMap.c */

/*******************************************************************************
* gqosSetVIDFPriOverride
*
* DESCRIPTION:
*        This routine sets VID Frame Priority Override. When this feature is enabled,
*        VID Frame priority overrides can occur on this port.
*        VID Frame priority override occurs when the determined VID of a frame 
*        results in a VTU entry whose useVIDFPri override field is set to GT_TRUE.
*        When this occurs the VIDFPri value assigned to the frame's VID (in the 
*        VTU Table) is used to overwrite the frame's previously determined frame 
*        priority. If the frame egresses tagged the priority in the frame will be
*        this new VIDFPri value. This function does not affect the egress queue
*        priority (QPri) the frame is switched into.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE for VID Frame Priority Override,
*               GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gqosSetVIDFPriOverride
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port,
    IN GT_BOOL        mode
);

/*******************************************************************************
* gqosGetVIDFPriOverride
*
* DESCRIPTION:
*        This routine gets VID Frame Priority Override. When this feature is enabled,
*        VID Frame priority overrides can occur on this port.
*        VID Frame priority override occurs when the determined VID of a frame 
*        results in a VTU entry whose useVIDFPri override field is set to GT_TRUE.
*        When this occurs the VIDFPri value assigned to the frame's VID (in the 
*        VTU Table) is used to overwrite the frame's previously determined frame 
*        priority. If the frame egresses tagged the priority in the frame will be
*        this new VIDFPri value. This function does not affect the egress queue
*        priority (QPri) the frame is switched into.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE for VID Frame Priority Override,
*               GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gqosGetVIDFPriOverride
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *mode
);

/*******************************************************************************
* gqosSetSAFPriOverride
*
* DESCRIPTION:
*        This routine sets Source Address(SA) Frame Priority Override. 
*        When this feature is enabled, SA Frame priority overrides can occur on 
*        this port.
*        SA ATU Frame priority override occurs when the determined source address
*        of a frame results in an ATU hit where the SA's MAC address entry contains 
*        the useATUFPri field set to GT_TRUE.
*        When this occurs the ATUFPri value assigned to the frame's SA (in the 
*        ATU Table) is used to overwrite the frame's previously determined frame 
*        priority. If the frame egresses tagged the priority in the frame will be
*        this new ATUFPri value. This function does not affect the egress queue
*        priority (QPri) the frame is switched into.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE for SA Frame Priority Override,
*               GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gqosSetSAFPriOverride
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    IN  GT_BOOL        mode
);

/*******************************************************************************
* gqosGetSAFPriOverride
*
* DESCRIPTION:
*        This routine gets Source Address(SA) Frame Priority Override. 
*        When this feature is enabled, SA Frame priority overrides can occur on 
*        this port.
*        SA ATU Frame priority override occurs when the determined source address
*        of a frame results in an ATU hit where the SA's MAC address entry contains 
*        the useATUFPri field set to GT_TRUE.
*        When this occurs the ATUFPri value assigned to the frame's SA (in the 
*        ATU Table) is used to overwrite the frame's previously determined frame 
*        priority. If the frame egresses tagged the priority in the frame will be
*        this new ATUFPri value. This function does not affect the egress queue
*        priority (QPri) the frame is switched into.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE for SA Frame Priority Override,
*               GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gqosGetSAFPriOverride
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *mode
);

/*******************************************************************************
* gqosSetDAFPriOverride
*
* DESCRIPTION:
*        This routine sets Destination Address(DA) Frame Priority Override. 
*        When this feature is enabled, DA Frame priority overrides can occur on 
*        this port.
*        DA ATU Frame priority override occurs when the determined destination address
*        of a frame results in an ATU hit where the DA's MAC address entry contains 
*        the useATUFPri field set to GT_TRUE.
*        When this occurs the ATUFPri value assigned to the frame's DA (in the 
*        ATU Table) is used to overwrite the frame's previously determined frame 
*        priority. If the frame egresses tagged the priority in the frame will be
*        this new ATUFPri value. This function does not affect the egress queue
*        priority (QPri) the frame is switched into.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE for DA Frame Priority Override,
*               GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gqosSetDAFPriOverride
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    IN  GT_BOOL        mode
);

/*******************************************************************************
* gqosGetDAFPriOverride
*
* DESCRIPTION:
*        This routine gets Destination Address(DA) Frame Priority Override. 
*        When this feature is enabled, DA Frame priority overrides can occur on 
*        this port.
*        DA ATU Frame priority override occurs when the determined destination address
*        of a frame results in an ATU hit where the DA's MAC address entry contains 
*        the useATUFPri field set to GT_TRUE.
*        When this occurs the ATUFPri value assigned to the frame's DA (in the 
*        ATU Table) is used to overwrite the frame's previously determined frame 
*        priority. If the frame egresses tagged the priority in the frame will be
*        this new ATUFPri value. This function does not affect the egress queue
*        priority (QPri) the frame is switched into.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE for DA Frame Priority Override,
*               GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gqosGetDAFPriOverride
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *mode
);

/*******************************************************************************
* gqosSetVTUQPriOverride
*
* DESCRIPTION:
*        This routine sets VTU Queue Priority Override. 
*        When this feature is enabled, VTU queue priority overrides can occur on 
*		 this port.
*        When this feature is disabled, normal frame priority processing occurs.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE for VTU Queue Priority Override,
*               GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gqosSetVTUQPriOverride
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    IN  GT_BOOL        mode
);

/*******************************************************************************
* gqosGetVTUQPriOverride
*
* DESCRIPTION:
*        This routine sets VTU Queue Priority Override. 
*        When this feature is enabled, VTU queue priority overrides can occur on 
*		 this port.
*        When this feature is disabled, normal frame priority processing occurs.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE for VTU Queue Priority Override,
*               GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gqosGetVTUQPriOverride
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *mode
);

/*******************************************************************************
* gqosSetVTUFPriOverride
*
* DESCRIPTION:
*        This routine sets VTU Frame Priority Override. 
*        When this feature is enabled, VTU frame priority overrides can occur on 
*		 this port.
*        When this feature is disabled, normal frame priority processing occurs.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE for VTU Frame Priority Override,
*               GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gqosSetVTUFPriOverride
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    IN  GT_BOOL        mode
);

/*******************************************************************************
* gqosGetVTUFPriOverride
*
* DESCRIPTION:
*        This routine sets VTU Frame Priority Override. 
*        When this feature is enabled, VTU frame priority overrides can occur on 
*		 this port.
*        When this feature is disabled, normal frame priority processing occurs.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE for VTU Frame Priority Override,
*               GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gqosGetVTUFPriOverride
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *mode
);

/*******************************************************************************
* gqosSetVIDQPriOverride
*
* DESCRIPTION:
*        This routine sets VID Queue Priority Override. When this feature is enabled,
*        VID Queue priority overrides can occur on this port.
*        VID Queue priority override occurs when the determined VID of a frame 
*        results in a VTU entry whose useVIDQPri override field is set to GT_TRUE.
*        When this occurs the VIDQPri value assigned to the frame's VID (in the 
*        VTU Table) is used to overwrite the frame's previously determined queue 
*        priority. If the frame egresses tagged the priority in the frame will not
*        be modified by this new VIDQPri value. This function affects the egress
*        queue priority (QPri) the frame is switched into.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE for VID Queue Priority Override,
*               GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gqosSetVIDQPriOverride
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port,
    IN GT_BOOL        mode
);

/*******************************************************************************
* gqosGetVIDQPriOverride
*
* DESCRIPTION:
*        This routine gets VID Queue Priority Override. When this feature is enabled,
*        VID Queue priority overrides can occur on this port.
*        VID Queue priority override occurs when the determined VID of a frame 
*        results in a VTU entry whose useVIDQPri override field is set to GT_TRUE.
*        When this occurs the VIDQPri value assigned to the frame's VID (in the 
*        VTU Table) is used to overwrite the frame's previously determined queue 
*        priority. If the frame egresses tagged the priority in the frame will not
*        be modified by this new VIDQPri value. This function affects the egress
*        queue priority (QPri) the frame is switched into.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE for VID Queue Priority Override,
*               GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gqosGetVIDQPriOverride
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *mode
);

/*******************************************************************************
* gqosSetSAQPriOverride
*
* DESCRIPTION:
*        This routine sets Source Address(SA) Queue Priority Override. 
*        When this feature is enabled, SA Queue priority overrides can occur on 
*        this port.
*        SA ATU Queue priority override occurs when the determined source address
*        of a frame results in an ATU hit where the SA's MAC address entry contains 
*        the useATUQPri field set to GT_TRUE.
*        When this occurs the ATUQPri value assigned to the frame's SA (in the 
*        ATU Table) is used to overwrite the frame's previously determined queue 
*        priority. If the frame egresses tagged the priority in the frame will not
*        be modified by this new ATUQPri value. This function affects the egress
*        queue priority (QPri) the frame is switched into.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE for SA Queue Priority Override,
*               GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gqosSetSAQPriOverride
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    IN  GT_BOOL        mode
);

/*******************************************************************************
* gqosGetSAQPriOverride
*
* DESCRIPTION:
*        This routine gets Source Address(SA) Queue Priority Override. 
*        When this feature is enabled, SA Queue priority overrides can occur on 
*        this port.
*        SA ATU Queue priority override occurs when the determined source address
*        of a frame results in an ATU hit where the SA's MAC address entry contains 
*        the useATUQPri field set to GT_TRUE.
*        When this occurs the ATUQPri value assigned to the frame's SA (in the 
*        ATU Table) is used to overwrite the frame's previously determined queue 
*        priority. If the frame egresses tagged the priority in the frame will not
*        be modified by this new ATUQPri value. This function affects the egress
*        queue priority (QPri) the frame is switched into.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE for SA Queue Priority Override,
*               GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gqosGetSAQPriOverride
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *mode
);

/*******************************************************************************
* gqosSetDAQPriOverride
*
* DESCRIPTION:
*        This routine sets Destination Address(DA) Queue Priority Override. 
*        When this feature is enabled, DA Queue priority overrides can occur on 
*        this port.
*        DA ATU Queue priority override occurs when the determined destination address
*        of a frame results in an ATU hit where the DA's MAC address entry contains 
*        the useATUQPri field set to GT_TRUE.
*        When this occurs the ATUQPri value assigned to the frame's DA (in the 
*        ATU Table) is used to overwrite the frame's previously determined queue
*        priority. If the frame egresses tagged the priority in the frame will not
*        be modified by this new ATUQPri value. This function affects the egress
*        queue priority (QPri) the frame is switched into.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE for DA Queue Priority Override,
*               GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gqosSetDAQPriOverride
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    IN  GT_BOOL        mode
);

/*******************************************************************************
* gqosGetDAQPriOverride
*
* DESCRIPTION:
*        This routine sets Destination Address(DA) Queue Priority Override. 
*        When this feature is enabled, DA Queue priority overrides can occur on 
*        this port.
*        DA ATU Queue priority override occurs when the determined destination address
*        of a frame results in an ATU hit where the DA's MAC address entry contains 
*        the useATUQPri field set to GT_TRUE.
*        When this occurs the ATUQPri value assigned to the frame's DA (in the 
*        ATU Table) is used to overwrite the frame's previously determined queue
*        priority. If the frame egresses tagged the priority in the frame will not
*        be modified by this new ATUQPri value. This function affects the egress
*        queue priority (QPri) the frame is switched into.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE for DA Queue Priority Override,
*               GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gqosGetDAQPriOverride
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *mode
);

/*******************************************************************************
* gqosSetARPQPriOverride
*
* DESCRIPTION:
*        This routine sets ARP Queue Priority Override. 
*        When this feature is enabled, ARP Queue priority overrides can occur on 
*        this port.
*        ARP Queue priority override occurs for all ARP frames.
*        When this occurs, the frame's previously determined egress queue priority
*        will be overwritten with ArpQPri.
*        If the frame egresses tagged the priority in the frame will not
*        be modified. When used, the two bits of the ArpQPri priority determine the
*        egress queue the frame is switched into.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE for ARP Queue Priority Override,
*               GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gqosSetARPQPriOverride
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    IN  GT_BOOL        mode
);

/*******************************************************************************
* gqosGetARPQPriOverride
*
* DESCRIPTION:
*        This routine sets ARP Queue Priority Override. 
*        When this feature is enabled, ARP Queue priority overrides can occur on 
*        this port.
*        ARP Queue priority override occurs for all ARP frames.
*        When this occurs, the frame's previously determined egress queue priority
*        will be overwritten with ArpQPri.
*        If the frame egresses tagged the priority in the frame will not
*        be modified. When used, the two bits of the ArpQPri priority determine the
*        egress queue the frame is switched into.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE for ARP Queue Priority Override,
*               GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gqosGetARPQPriOverride
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *mode
);


/*******************************************************************************
* gqosSetQPriValue
*
* DESCRIPTION:
*       This routine sets Queue priority value to used when forced.
*        When ForceQPri is enabled (gqosSetForceQPri), all frames entering this port
*        are mapped to the priority queue defined in this value, unless a VTU, SA,
*        DA or ARP priority override occurs. The Frame's priority (FPri) is not
*        effected by this value.
*
* INPUTS:
*       port - the logical port number.
*       pri  - Queue priority value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*        GT_BAD_PARAM - if pri > 3
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gqosSetQPriValue
(
    IN  GT_QD_DEV  *dev,
    IN  GT_LPORT   port,
    IN  GT_U8      pri
);

/*******************************************************************************
* gqosGetQPriValue
*
* DESCRIPTION:
*       This routine gets Queue priority value to used when forced.
*        When ForceQPri is enabled (gqosSetForceQPri), all frames entering this port
*        are mapped to the priority queue defined in this value, unless a VTU, SA,
*        DA or ARP priority override occurs. The Frame's priority (FPri) is not
*        effected by this value.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       pri  - Queue priority value
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gqosGetQPriValue
(
    IN  GT_QD_DEV  *dev,
    IN  GT_LPORT   port,
    OUT GT_U8      *pri
);

/*******************************************************************************
* gqosSetForceQPri
*
* DESCRIPTION:
*       This routine enables/disables forcing Queue priority.
*        When ForceQPri is disabled, normal priority queue mapping is used on all 
*        ingressing frames entering this port. When it's enabled, all frames
*        entering this port are mapped to the QPriValue (gqosSetQPriValue), unless
*        a VTU, SA, DA or ARP priority override occurs. The frame's priorty (FPri)
*        is not effected by this feature.
*
* INPUTS:
*       port - the logical port number.
*       en   - GT_TRUE, to force Queue Priority,
*               GT_FALSE, otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gqosSetForceQPri
(
    IN  GT_QD_DEV  *dev,
    IN  GT_LPORT   port,
    IN  GT_BOOL    en
);

/*******************************************************************************
* gqosGetForceQPri
*
* DESCRIPTION:
*       This routine checks if forcing Queue priority is enabled.
*        When ForceQPri is disabled, normal priority queue mapping is used on all 
*        ingressing frames entering this port. When it's enabled, all frames
*        entering this port are mapped to the QPriValue (gqosSetQPriValue), unless
*        a VTU, SA, DA or ARP priority override occurs. The frame's priorty (FPri)
*        is not effected by this feature.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       en   - GT_TRUE, to force Queue Priority,
*               GT_FALSE, otherwise.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gqosGetForceQPri
(
    IN  GT_QD_DEV  *dev,
    IN  GT_LPORT   port,
    OUT GT_BOOL    *en
);

/*******************************************************************************
* gqosSetDefFPri
*
* DESCRIPTION:
*       This routine sets the default frame priority (0 ~ 7).
*        This priority is used as the default frame priority (FPri) to use when 
*        no other priority information is available.
*
* INPUTS:
*       port - the logical port number
*       pri  - default frame priority
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*        GT_BAD_PARAM - if pri > 7
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gqosSetDefFPri
(
    IN  GT_QD_DEV  *dev,
    IN  GT_LPORT   port,
    IN  GT_U8      pri
);

/*******************************************************************************
* gqosGetDefFPri
*
* DESCRIPTION:
*       This routine gets the default frame priority (0 ~ 7).
*        This priority is used as the default frame priority (FPri) to use when 
*        no other priority information is available.
*
* INPUTS:
*       port - the logical port number
*
* OUTPUTS:
*       pri  - default frame priority
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gqosGetDefFPri
(
    IN  GT_QD_DEV  *dev,
    IN  GT_LPORT   port,
    OUT GT_U8      *pri
);


/*******************************************************************************
* gqosSetArpQPri
*
* DESCRIPTION:
*       This routine sets ARP queue Priority to use for ARP QPri Overridden 
*        frames. When a ARP frame is received on a por tthat has its ARP 
*        QPriOVerride is enabled, the QPri assigned to the frame comes from
*        this value
*
* INPUTS:
*       pri - ARP Queue Priority (0 ~ 3)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*        GT_BAD_PARAM - if pri > 3
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gqosSetArpQPri
(
    IN  GT_QD_DEV *dev,
    IN  GT_U8     pri
);


/*******************************************************************************
* gqosGetArpQPri
*
* DESCRIPTION:
*       This routine gets ARP queue Priority to use for ARP QPri Overridden 
*        frames. When a ARP frame is received on a por tthat has its ARP 
*        QPriOVerride is enabled, the QPri assigned to the frame comes from
*        this value
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       pri - ARP Queue Priority (0 ~ 3)
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gqosGetArpQPri
(
    IN  GT_QD_DEV *dev,
    OUT GT_U8     *pri
);


/* gtSysCtrl.c */

/*******************************************************************************
* gsysSetUsePortSchedule
*
* DESCRIPTION:
*       This routine sets per port scheduling mode
*
* INPUTS:
*       en - GT_TRUE enables per port scheduling, 
*             GT_FALSE disable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gsysSetUsePortSchedule
(
    IN  GT_QD_DEV *dev,
    IN  GT_BOOL   en
);

/*******************************************************************************
* gsysGetUsePortSchedule
*
* DESCRIPTION:
*       This routine gets per port scheduling mode
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       en - GT_TRUE enables per port scheduling, 
*             GT_FALSE disable.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gsysGetUsePortSchedule
(
    IN  GT_QD_DEV *dev,
    OUT GT_BOOL   *en
);

/*******************************************************************************
* gsysSetOldHader
*
* DESCRIPTION:
*       This routine sets Egress Old Header.
*        When this feature is enabled and frames are egressed with a Marvell Header, 
*        the format of the Header is slightly modified to be backwards compatible 
*        with previous devices that used the original Header. Specifically, bit 3
*        of the Header's 2nd octet is cleared to a zero such that only FPri[2:1]
*        is available in the Header.
*
* INPUTS:
*       en - GT_TRUE to enable Old Header Mode,
*             GT_FALSE to disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gsysSetOldHader
(
    IN  GT_QD_DEV *dev,
    IN  GT_BOOL   en
);

/*******************************************************************************
* gsysGetOldHader
*
* DESCRIPTION:
*       This routine gets Egress Old Header.
*        When this feature is enabled and frames are egressed with a Marvell Header, 
*        the format of the Header is slightly modified to be backwards compatible 
*        with previous devices that used the original Header. Specifically, bit 3
*        of the Header's 2nd octet is cleared to a zero such that only FPri[2:1]
*        is available in the Header.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       en - GT_TRUE to enable Old Header Mode,
*             GT_FALSE to disable
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gsysGetOldHader
(
    IN  GT_QD_DEV *dev,
    OUT GT_BOOL   *en
);

/*******************************************************************************
* gsysSetRecursiveStrippingDisable
*
* DESCRIPTION:
*       This routine determines if recursive tag stripping feature needs to be
*        disabled.
*
* INPUTS:
*       en - GT_TRUE to disable Recursive Tag Stripping,
*             GT_FALSE to enable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gsysSetRecursiveStrippingDisable
(
    IN  GT_QD_DEV *dev,
    IN  GT_BOOL   en
);

/*******************************************************************************
* gsysGetRecursiveStrippingDisable
*
* DESCRIPTION:
*       This routine checks if recursive tag stripping feature is disabled.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       en - GT_TRUE, if Recursive Tag Stripping is disabled,
*             GT_FALSE, otherwise
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gsysGetRecursiveStrippingDisable
(
    IN  GT_QD_DEV *dev,
    OUT GT_BOOL   *en
);

/*******************************************************************************
* gsysSetCPUPort
*
* DESCRIPTION:
*       This routine sets CPU Port where Rsvd2Cpu frames and IGMP/MLD Snooped 
*        frames are destined.
*
* INPUTS:
*       cpuPort - CPU Port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gsysSetCPUPort
(
    IN  GT_QD_DEV *dev,
    IN  GT_LPORT  cpuPort
);

/*******************************************************************************
* gsysGetCPUPort
*
* DESCRIPTION:
*       This routine gets CPU Port where Rsvd2Cpu frames and IGMP/MLD Snooped 
*        frames are destined.
*
* INPUTS:
*       cpuPort - CPU Port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gsysGetCPUPort
(
    IN  GT_QD_DEV *dev,
    OUT GT_LPORT  *cpuPort
);



/* gtSysStatus.c */

/*******************************************************************************
* gsysGetFreeQSize
*
* DESCRIPTION:
*       This routine gets Free Queue Counter. This counter reflects the 
*        current number of unalllocated buffers available for all the ports.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       count - Free Queue Counter
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gsysGetFreeQSize
(
    IN  GT_QD_DEV    *dev,
    OUT GT_U16         *count
);

/*******************************************************************************
                New APIs in DSDT 2.6
*******************************************************************************/

/* gtBrgFdb.c */

/*******************************************************************************
* gfdbSetPortAtuLearnLimit
*
* DESCRIPTION:
*       Port's auto learning limit. When the limit is non-zero value, the number
*        of MAC addresses that can be learned on this port are limited to the value
*        specified in this API. When the learn limit has been reached any frame 
*        that ingresses this port with a source MAC address not already in the 
*        address database that is associated with this port will be discarded. 
*        Normal auto-learning will resume on the port as soon as the number of 
*        active unicast MAC addresses associated to this port is less than the 
*        learn limit.
*        CPU directed ATU Load, Purge, or Move will not have any effect on the 
*        learn limit.
*        This feature is disabled when the limit is zero.
*        The following care is needed when enabling this feature:
*            1) disable learning on the ports
*            2) flush all non-static addresses in the ATU
*            3) define the desired limit for the ports
*            4) re-enable learing on the ports
*
* INPUTS:
*       port  - logical port number
*       limit - auto learning limit ( 0 ~ 255 )
*                                              
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*        GT_BAD_PARAM - if limit > 0xFF
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gfdbSetPortAtuLearnLimit
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT      port,
    IN  GT_U32       limit
);


/*******************************************************************************
* gfdbGetPortAtuLearnCnt
*
* DESCRIPTION:
*       Read the current number of active unicast MAC addresses associated with 
*        the given port. This counter (LearnCnt) is held at zero if learn limit
*        (gfdbSetPortAtuLearnLimit API) is set to zero.
*
* INPUTS:
*       port  - logical port number
*                                              
* OUTPUTS:
*       count - current auto learning count
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gfdbGetPortAtuLearnCnt
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT      port,
    IN  GT_U32       *count
);

/*******************************************************************************
* gfdbGetAtuAllCount
*
* DESCRIPTION:
*       Counts all entries in the Address Translation Unit.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       count - number of valid entries.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gfdbGetAtuAllCount
(
    IN  GT_QD_DEV     *dev,
    OUT GT_U32         *count
);

/*******************************************************************************
* gfdbGetAtuAllCountInDBNum
*
* DESCRIPTION:
*       Counts all entries in the defined FID (or DBNum).
*
* INPUTS:
*       dbNum - DBNum of FID
*
* OUTPUTS:
*       count - number of valid entries in FID (or DBNum).
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gfdbGetAtuAllCountInDBNum
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U32         dbNum,
    OUT GT_U32         *count
);

/*******************************************************************************
* gfdbGetAtuDynamicCountInDBNum
*
* DESCRIPTION:
*       Counts all non-static entries in the defined FID (or DBNum).
*
* INPUTS:
*       dbNum - DBNum or FID
*
* OUTPUTS:
*       count - number of valid non-static entries in FID (or DBNum).
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gfdbGetAtuDynamicCountInDBNum
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U32         dbNum,
    OUT GT_U32         *count
);


/* gtBrgStu.c */

/*******************************************************************************
* gstuGetEntryCount
*
* DESCRIPTION:
*       Gets the current number of valid entries in the STU table
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       numEntries - number of STU entries.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gstuGetEntryCount
(
    IN  GT_QD_DEV *dev,
    OUT GT_U32    *numEntries
);

/*******************************************************************************
* gstuGetEntryFirst
*
* DESCRIPTION:
*       Gets first lexicographic entry from the STU.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       stuEntry - find the first valid STU entry.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NO_SUCH - table is empty.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gstuGetEntryFirst
(
    IN  GT_QD_DEV       *dev,
    OUT GT_STU_ENTRY    *stuEntry
);

/*******************************************************************************
* gstuGetEntryNext
*
* DESCRIPTION:
*       Gets next lexicographic STU entry from the specified SID.
*
* INPUTS:
*       stuEntry - the SID to start the search.
*
* OUTPUTS:
*       stuEntry - next STU entry.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - on error or entry does not exist.
*       GT_NO_SUCH - no more entries.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gstuGetEntryNext
(
    IN  GT_QD_DEV       *dev,
    INOUT GT_STU_ENTRY  *stuEntry
);

/*******************************************************************************
* gstuFindSidEntry
*
* DESCRIPTION:
*       Find STU entry for a specific SID, it will return the entry, if found, 
*       along with its associated data 
*
* INPUTS:
*       stuEntry - contains the SID to searche for 
*
* OUTPUTS:
*       found    - GT_TRUE, if the appropriate entry exists.
*       stuEntry - the entry parameters.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - on error or entry does not exist.
*       GT_NO_SUCH - no such entry.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gstuFindSidEntry
(
    IN  GT_QD_DEV       *dev,
    INOUT GT_STU_ENTRY  *stuEntry,
    OUT GT_BOOL         *found
);

/*******************************************************************************
* gstuAddEntry
*
* DESCRIPTION:
*       Creates or update the entry in STU table based on user input.
*
* INPUTS:
*       stuEntry    - stu entry to insert to the STU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_FAIL           - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gstuAddEntry
(
    IN  GT_QD_DEV       *dev,
    IN  GT_STU_ENTRY    *stuEntry
);

/*******************************************************************************
* gstuDelEntry
*
* DESCRIPTION:
*       Deletes STU entry specified by user.
*
* INPUTS:
*       stuEntry - the STU entry to be deleted 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gstuDelEntry
(
    IN  GT_QD_DEV       *dev,
    IN  GT_STU_ENTRY     *stuEntry
);


/* gtCCPVT.c */

/*******************************************************************************
* gpvtInitialize
*
* DESCRIPTION:
*       This routine initializes the PVT Table to all one's (initial state)
*
* INPUTS:
*        None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gpvtInitialize
(
    IN  GT_QD_DEV     *dev
);

/*******************************************************************************
* gpvtWritePVTData
*
* DESCRIPTION:
*       This routine write Cross Chip Port Vlan Data.
*        Cross chip Port VLAN Data used as a bit mask to limit where cross chip
*        frames can egress (in chip Port VLANs are masked using gvlnSetPortVlanPorts
*        API). Cross chip frames are Forward frames that ingress a DSA or Ether 
*        Type DSA port (see gprtSetFrameMode API). Bit 0 is a mask for port 0, 
*        bit 1 for port 1, etc. When a port's mask bit is one, frames are allowed 
*        to egress that port on this device. When a port's mask bit is zero,
*        frames are not allowed to egress that port on this device.
*
*        The Cross Chip Port VLAN Table is accessed by ingressing frames based
*        upon the original source port of the frame using the Forward frame's DSA tag
*        fields Src_Dev, Src_Port/Src_Trunk and Src_Is_Trunk. The 1 entry of the 512
*        that is accessed by the frame is:
*            If 5 Bit Port (in Global 2, offset 0x1D) = 0:
*                If Src_Is_Trunk = 0   Src_Dev[4:0], Src_Port[3:0]119
*                If Src_Is_Trunk = 1   Device Number (global offset 0x1C), Src_Trunk[3:0]
*            If 5 Bit Port (in Global 2, offset 0x1D) = 1:
*                If Src_Is_Trunk = 0   Src_Dev[3:0], Src_Port[4:0]120
*                If Src_Is_Trunk = 1   Device Number[3:0], Src_Trunk[4:0]
*
*        Cross chip port VLANs with Trunks are supported in the table where this
*        device's entries would be stored (defined by this device's Device Number).
*        This portion of the table is available for Trunk entries because this device's
*        port VLAN mappings to ports inside this device are masked by the port's
*        VLAN Table (see gvlnSetPortVlanPorts API).
*
*
* INPUTS:
*        pvtPointer - pointer to the desired entry of PVT (0 ~ 511)
*        pvtData    - Cross Chip Port Vlan Data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gpvtWritePVTData
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U32        pvtPointer,
    IN  GT_U32        pvtData
);

/*******************************************************************************
* gpvtReadPVTData
*
* DESCRIPTION:
*       This routine reads Cross Chip Port Vlan Data.
*        Cross chip Port VLAN Data used as a bit mask to limit where cross chip
*        frames can egress (in chip Port VLANs are masked using gvlnSetPortVlanPorts
*        API). Cross chip frames are Forward frames that ingress a DSA or Ether 
*        Type DSA port (see gprtSetFrameMode API). Bit 0 is a mask for port 0, 
*        bit 1 for port 1, etc. When a port's mask bit is one, frames are allowed 
*        to egress that port on this device. When a port's mask bit is zero,
*        frames are not allowed to egress that port on this device.
*
*        The Cross Chip Port VLAN Table is accessed by ingressing frames based
*        upon the original source port of the frame using the Forward frame's DSA tag
*        fields Src_Dev, Src_Port/Src_Trunk and Src_Is_Trunk. The 1 entry of the 512
*        that is accessed by the frame is:
*            If 5 Bit Port (in Global 2, offset 0x1D) = 0:
*                If Src_Is_Trunk = 0   Src_Dev[4:0], Src_Port[3:0]119
*                If Src_Is_Trunk = 1   Device Number (global offset 0x1C), Src_Trunk[3:0]
*            If 5 Bit Port (in Global 2, offset 0x1D) = 1:
*                If Src_Is_Trunk = 0   Src_Dev[3:0], Src_Port[4:0]120
*                If Src_Is_Trunk = 1   Device Number[3:0], Src_Trunk[4:0]
*
*        Cross chip port VLANs with Trunks are supported in the table where this
*        device's entries would be stored (defined by this device's Device Number).
*        This portion of the table is available for Trunk entries because this device's
*        port VLAN mappings to ports inside this device are masked by the port's
*        VLAN Table (see gvlnSetPortVlanPorts API).
*
*
* INPUTS:
*        pvtPointer - pointer to the desired entry of PVT (0 ~ 511)
*
* OUTPUTS:
*        pvtData    - Cross Chip Port Vlan Data
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gpvtReadPVTData
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U32        pvtPointer,
    OUT GT_U32        *pvtData
);


/* gtEvents.c */

/*******************************************************************************
* geventGetDevIntStatus
*
* DESCRIPTION:
*         Check to see which device interrupts (WatchDog, JamLimit, Duplex Mismatch, and
*        SERDES Link Int) have occurred.
*
* INPUTS:
*       intType - the type of interrupt which causes an interrupt.
*                  any combination of 
*                    GT_DEV_INT_WATCHDOG,
*                    GT_DEV_INT_JAMLIMIT,
*                    GT_DEV_INT_DUPLEX_MISMATCH,
*                    GT_DEV_INT_SERDES_LINK
*        port    - logical port where GT_DEV_INT_DUPLEX_MISMATCH occurred.
*                  valid only if GT_DEV_INT_DUPLEX_MISMATCH is set in intType.
*        serInt - SERDES port list where GT_DEV_INT_SERDES_LINK interrupt is
*                  asserted. It's in vector format, Bit 10 is for port 10, 
*                  Bit 9 is for port 9, etc.
*                  valid only if GT_DEV_INT_SERDES_LINK bit is set in intType.
*                  These bits are only valid of the port that is in 1000Base-X mode.
*
* OUTPUTS:
*         None.
*
* RETURNS:
*         GT_OK - on success
*         GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS geventGetDevIntStatus
(
    IN  GT_QD_DEV             *dev,
    OUT GT_DEV_INT_STATUS    *devIntStatus
);

/*******************************************************************************
* geventSetAgeOutIntEn
*
* DESCRIPTION:
*        Interrupt on Age Out. When aging is enabled, all non-static address 
*        entries in the ATU's address database are periodically aged.
*        When this feature is set to GT_TRUE and an entry associated with this 
*        port is aged out, an AgeOutViolation will be captured for that entry.
*
* INPUTS:
*        port - the logical port number
*        mode - GT_TRUE to enable Age Out Interrupt,
*               GT_FALUSE to disable
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS geventSetAgeOutIntEn
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    IN  GT_BOOL        mode
);

/*******************************************************************************
* geventGetAgeOutIntEn
*
* DESCRIPTION:
*        Interrupt on Age Out. When aging is enabled, all non-static address 
*        entries in the ATU's address database are periodically aged.
*        When this feature is set to GT_TRUE and an entry associated with this 
*        port is aged out, an AgeOutViolation will be captured for that entry.
*
* INPUTS:
*        port - the logical port number
*
* OUTPUTS:
*        mode - GT_TRUE, if Age Out Interrupt is enabled
*               GT_FALUSE, otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS geventGetAgeOutIntEn
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *mode
);

/*******************************************************************************
* geventSetOverLimitInt
*
* DESCRIPTION:
*        This routine enables/disables Over Limit Interrupt for a port.
*        If it's enabled, an ATU Miss violation will be generated when port auto
*        learn reached the limit(refer to gfdbGetPortAtuLimitReached API).
*
* INPUTS:
*        port - the logical port number
*        mode - GT_TRUE to enable Over Limit Interrupt,
*               GT_FALUSE to disable
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS geventSetOverLimitInt
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    IN  GT_BOOL        mode
);

/*******************************************************************************
* geventGetOverLimitInt
*
* DESCRIPTION:
*        This routine enables/disables Over Limit Interrupt for a port.
*        If it's enabled, an ATU Miss violation will be generated when port auto
*        learn reached the limit(refer to gfdbSetPortAtuLearnLimit API).
*
* INPUTS:
*        port - the logical port number
*
* OUTPUTS:
*        mode - GT_TRUE to enable Over Limit Interrupt,
*               GT_FALUSE to disable
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS geventGetOverLimitInt
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *mode
);

/*******************************************************************************
* geventGetPortAtuLimitReached
*
* DESCRIPTION:
*       This routine checks if learn limit has been reached.
*        When it reached, the port can no longer auto learn any more MAC addresses
*        because the address learn limit set on this port has been reached.
*
* INPUTS:
*       port  - logical port number
*                                              
* OUTPUTS:
*       limit - GT_TRUE, if limit has been reached
*                GT_FALSE, otherwise
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS geventGetPortAtuLimitReached
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT      port,
    IN  GT_BOOL       *limit
);

/*******************************************************************************
* eventSetDevInt
*
* DESCRIPTION:
*        Device Interrupt.
*        The following device interrupts are supported:
*            GT_DEV_INT_WATCHDOG    - 
*                WatchDog event interrupt (WatchDog event can be configured with 
*                gwdSetEvent API)
*            GT_DEV_INT_JAMLIMIT    -
*                any of the ports detect an Ingress Jam Limit violation
*                (see gprtSetPauseLimitIn API)
*            GT_DEV_INT_DUPLEX_MISMATCH - 
*                any of the ports detect a duplex mismatch (i.e., the local port is 
*                in half duplex mode while the link partner is in full duplex mode)
*            GT_DEV_INT_SERDES_LINK - 
*                SERDES link chage interrupt.
*                An interrupt occurs when a SERDES port changes link status
*                (link up or link down)
*            
*        If any of the above events is enabled, GT_DEVICE_INT interrupt will
*        be asserted by the enabled event when GT_DEV_INT is enabled with 
*        eventSetActive API.
*        
* INPUTS:
*        devInt - GT_DEV_INT
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS eventSetDevInt
(
    IN  GT_QD_DEV    *dev,
    IN  GT_DEV_EVENT    *devInt
);

/*******************************************************************************
* gwdSetEvent
*
* DESCRIPTION:
*        Watch Dog Event.
*        The following Watch Dog events are supported:
*            GT_WD_QC  - Queue Controller Watch Dog enable.
*                        When enabled, the QC's watch dog circuit checks for link
*                        list errors and any errors found in the QC.
*            GT_WD_EGRESS - Egress Watch Dog enable.
*                        When enabled, each port's egress circuit checks for problems
*                        between the port and the Queue Controller.
*            GT_WD_FORCE - Force a Watch Dog event.
*            
*        If any of the above events is enabled, GT_DEVICE_INT interrupt will
*        be asserted by the enabled WatchDog event when GT_DEV_INT_WATCHDOG is
*        enabled with eventSetDevActive API and GT_DEV_INT is enabled with 
*        eventSetActive API.
*        
* INPUTS:
*        wdEvent - Watch Dog Events
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gwdSetEvent
(
    IN  GT_QD_DEV    *dev,
    IN  GT_U32        wdEvent
);

/*******************************************************************************
* gwdSetSWResetOnWD
*
* DESCRIPTION:
*        SWReset on Watch Dog Event.
*        When this feature is enabled, any enabled watch dog event (gwdSetEvent API) 
*        will automatically reset the switch core's datapath just as if gsysSwReset
*        API is called.
*
*        The Watch Dog History (gwdGetHistory API) won't be cleared by this 
*        automatic SWReset. This allows the user to know if any watch dog event 
*        ever occurred even if the swich is configured to automatically recover 
*        from a watch dog.
*
*        When this feature is disabled, enabled watch dog events will not cause a
*        SWReset.
*
* INPUTS:
*        en   - GT_TRUE to enable SWReset on WD
*               GT_FALUSE to disable
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gwdSetSWResetOnWD
(
    IN  GT_QD_DEV    *dev,
    IN  GT_BOOL        en
);

/*******************************************************************************
* gwdGetSWResetOnWD
*
* DESCRIPTION:
*        SWReset on Watch Dog Event.
*        When this feature is enabled, any enabled watch dog event (gwdSetEvent API) 
*        will automatically reset the switch core's datapath just as if gsysSwReset
*        API is called.
*
*        The Watch Dog History (gwdGetHistory API) won't be cleared by this 
*        automatic SWReset. This allows the user to know if any watch dog event 
*        ever occurred even if the swich is configured to automatically recover 
*        from a watch dog.
*
*        When this feature is disabled, enabled watch dog events will not cause a
*        SWReset.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        en   - GT_TRUE, if SWReset on WD is enabled
*               GT_FALUSE, otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gwdGetSWResetOnWD
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL        *en
);

/*******************************************************************************
* gwdGetHistory
*
* DESCRIPTION:
*        This routine retrieves Watch Dog history. They are
*
*        wdEvent - 
*            When it's set to GT_TRUE, some enabled Watch Dog event occurred.
*            The following events are possible:
*                QC WatchDog Event (GT_WD_QC)
*                Egress WatchDog Event (GT_WD_EGRESS)
*                Forced WatchDog Event (GT_WD_FORCE)
*        egressEvent -
*            If any port's egress logic detects an egress watch dog issue,
*            this field is set to GT_TRUE, regardless of the enabling GT_WD_EGRESS
*            event.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        history - GT_WD_EVENT_HISTORY structure
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gwdGetHistory
(
    IN  GT_QD_DEV            *dev,
    OUT GT_WD_EVENT_HISTORY    *history
);


/* gtPIRL2.c */

/*******************************************************************************
* pirl2DumpResource
*
* DESCRIPTION:
*       This function dumps IRL Resource register values.
*
* INPUTS:
*       irlPort  - physical port number.
*        irlRes   - bucket to be used (0 ~ 4).
*        dataLen  - data size.
*
* OUTPUTS:
*       data - IRL Resource data
*
* RETURNS:
*       GT_OK on success,
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pirl2DumpResource
(
    IN  GT_QD_DEV              *dev,
    IN    GT_U32                irlPort,
    IN    GT_U32                irlRes,
    IN    GT_U32                dataLen,
    OUT GT_U16                *data
);


/*******************************************************************************
* gpirl2WriteResource
*
* DESCRIPTION:
*       This routine writes resource bucket parameters to the given resource
*        of the port.
*
* INPUTS:
*       port     - logical port number.
*        irlRes   - bucket to be used (0 ~ 4).
*        pirlData - PIRL resource parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gpirl2WriteResource
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_U32        irlRes,
    IN  GT_PIRL2_DATA    *pirlData
);

/*******************************************************************************
* gpirl2ReadResource
*
* DESCRIPTION:
*       This routine retrieves IRL Parameter.
*
* INPUTS:
*       port     - logical port number.
*        irlRes   - bucket to be used (0 ~ 4).
*
* OUTPUTS:
*        pirlData - PIRL resource parameters.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gpirl2ReadResource
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_U32        irlRes,
    OUT GT_PIRL2_DATA    *pirlData
);

/*******************************************************************************
* gpirl2DisableResource
*
* DESCRIPTION:
*       This routine disables Ingress Rate Limiting for the given bucket.
*
* INPUTS:
*       port     - logical port number.
*        irlRes   - bucket to be used (0 ~ 4).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gpirl2DisableResource
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_U32        irlRes
);

/*******************************************************************************
* gpirl2SetCurTimeUpInt
*
* DESCRIPTION:
*       This function sets the current time update interval.
*        Please contact FAE for detailed information.
*
* INPUTS:
*       upInt - updata interval (0 ~ 7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gpirl2SetCurTimeUpInt
(
    IN  GT_QD_DEV              *dev,
    IN    GT_U32                upInt
);

/*******************************************************************************
* gpirl2WriteTSMResource
*
* DESCRIPTION:
*        This routine writes rate resource bucket parameters in Time Slot Metering
*        mode to the given resource of the port.
*
* INPUTS:
*        port     - logical port number.
*        irlRes   - bucket to be used (0 ~ 1).
*        pirlData - PIRL TSM resource parameters.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        Only Resource 0 and 1 can be supported for TSM Mode.
*
*******************************************************************************/
GT_STATUS gpirl2WriteTSMResource
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_U32        irlRes,
    IN  GT_PIRL2_TSM_DATA    *pirlData
);


/*******************************************************************************
* gpirl2ReadTSMResource
*
* DESCRIPTION:
*        This routine retrieves IRL Parameter.
*        Returned ingressRate would be rough number. Instead, customSetup will
*        have the exact configured value.
*
* INPUTS:
*        port     - logical port number.
*        irlRes   - bucket to be used (0 ~ 1).
*
* OUTPUTS:
*        pirlData - PIRL resource parameters.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        Only Resource 0 and 1 can be supported for TSM Mode.
*
*******************************************************************************/
GT_STATUS gpirl2ReadTSMResource
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_U32        irlRes,
    OUT GT_PIRL2_TSM_DATA    *pirlData
);

/* gtPIRL3.c */


/*******************************************************************************
* pirl3DumpResource
*
* DESCRIPTION:
*       This function dumps IRL Resource register values.
*
* INPUTS:
*       irlPort  - physical port number.
*        irlRes   - bucket to be used (0 ~ 4).
*        dataLen  - data size.
*
* OUTPUTS:
*       data - IRL Resource data
*
* RETURNS:
*       GT_OK on success,
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pirl3DumpResource
(
    IN  GT_QD_DEV              *dev,
    IN    GT_U32                irlPort,
    IN    GT_U32                irlRes,
    IN    GT_U32                dataLen,
    OUT GT_U16                *data
);


/*******************************************************************************
* gpirl3WriteResource
*
* DESCRIPTION:
*        This routine writes resource bucket parameters to the given resource
*        of the port.
*
* INPUTS:
*        port     - logical port number.
*        irlRes   - bucket to be used (0 ~ 7).
*                   IRLRes numbers 0x0 to 0x4 are IP/Storm Rate Limiting Resources.
*                   IRLRes numbers 0x5 to 0x7 are AVB Rate Limiting Resources
*        pirlData - PIRL resource parameters.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gpirl3WriteResource
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_U32        irlRes,
    IN  GT_PIRL3_DATA    *pirlData
);

/*******************************************************************************
* gpirl3ReadResource
*
* DESCRIPTION:
*        This routine retrieves IRL Parameter.
*
* INPUTS:
*        port     - logical port number.
*        irlRes   - bucket to be used (0 ~ 7).
*                   IRLRes numbers 0x0 to 0x4 are IP/Storm Rate Limiting Resources.
*                   IRLRes numbers 0x5 to 0x7 are AVB Rate Limiting Resources
*
* OUTPUTS:
*        pirlData - PIRL resource parameters.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gpirl3ReadResource
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_U32        irlRes,
    OUT GT_PIRL3_DATA    *pirlData
);

/*******************************************************************************
* gpirl3DisableResource
*
* DESCRIPTION:
*       This routine disables Ingress Rate Limiting for the given bucket.
*
* INPUTS:
*       port     - logical port number.
*        irlRes   - bucket to be used (0 ~ 7).
*                   IRLRes numbers 0x0 to 0x4 are IP/Storm Rate Limiting Resources.
*                   IRLRes numbers 0x5 to 0x7 are AVB Rate Limiting Resources
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gpirl3DisableResource
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_U32        irlRes
);

/*******************************************************************************
* gpirl3SetCurTimeUpInt
*
* DESCRIPTION:
*       This function sets the current time update interval.
*        Please contact FAE for detailed information.
*
* INPUTS:
*       upInt - updata interval (0 ~ 7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gpirl3SetCurTimeUpInt
(
    IN  GT_QD_DEV              *dev,
    IN    GT_U32                upInt
);

/*******************************************************************************
* gpirl3WriteTSMResource
*
* DESCRIPTION:
*        This routine writes rate resource bucket parameters in Time Slot Metering
*        mode to the given resource of the port.
*
* INPUTS:
*        port     - logical port number.
*        irlRes   - bucket to be used (0 ~ 1).
*        pirlData - PIRL TSM resource parameters.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        Only Resource 0 and 1 can be supported for TSM Mode.
*
*******************************************************************************/
GT_STATUS gpirl3WriteTSMResource
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_U32        irlRes,
    IN  GT_PIRL3_TSM_DATA    *pirlData
);

/*******************************************************************************
* gpirl3ReadTSMResource
*
* DESCRIPTION:
*        This routine retrieves IRL Parameter.
*        Returned ingressRate would be rough number. Instead, customSetup will
*        have the exact configured value.
*
* INPUTS:
*        port     - logical port number.
*        irlRes   - bucket to be used (0 ~ 1).
*
* OUTPUTS:
*        pirlData - PIRL resource parameters.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        Only Resource 0 and 1 can be supported for TSM Mode.
*
*******************************************************************************/
GT_STATUS gpirl3ReadTSMResource
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_U32        irlRes,
    OUT GT_PIRL3_TSM_DATA    *pirlData
);



















/* gtPolicy.c */

/*******************************************************************************
* gprtSetPolicy
*
* DESCRIPTION:
*       This routine sets the Policy for ports.
*        Supported Policies are defined as GT_FRAME_POLICY as follows:
*            FRAME_POLICY_NONE    - normal frame switching
*            FRAME_POLICY_MIRROR  - mirror (copy) frame to MirrorDest port
*            FRAME_POLICY_TRAP    - trap(re-direct) frame to the CPUDest port
*            FRAME_POLICY_DISCARD - discard(filter) the frame
*        Supported Policy types are defined as GT_POLICY_TYPE:
*            POLICY_TYPE_DA - DA Policy Mapping
*                DA Policy Mapping occurs when the DA of a frame is contained in 
*                the ATU address database with an Entry State that indicates Policy.
*            POLICY_TYPE_SA - SA Policy Mapping
*                SA Policy Mapping occurs when the SA of a frame is contained in 
*                the ATU address database with an Entry State that indicates Policy.
*            POLICY_TYPE_VTU - VTU Policy Mapping
*                VTU Policy Mapping occurs when the VID of a frame is contained in
*                the VTU database with the VidPolicy is enabled.
*            POLICY_TYPE_ETYPE - EtherType Policy Mapping
*                EType Policy Mapping occurs when the EtherType of a frame matches
*                the PortEType (see gprtSetPortEType API)
*            POLICY_TYPE_PPPoE - PPPoE Policy Mapping
*                PPPoE Policy Mapping occurs when the EtherType of a frame matches 0x8863
*            POLICY_TYPE_VBAS - VBAS Policy Mapping
*                VBAS Policy Mapping occurs when the EtherType of a frame matches 0x8200
*            POLICY_TYPE_OPT82 - DHCP Option 82 Policy Mapping
*                DHCP Option 82 Policy Mapping occurs when the ingressing frame is an
*                IPv4 UDP with a UDP Destination port = 0x0043 or 0x0044, or an
*                IPv6 UDP with a UDP Destination port = 0x0223 or 0x0222
*            POLICY_TYPE_UDP - UDP Policy Mapping
*                UDP Policy Mapping occurs when the ingressing frame is
*                a Broadcast IPv4 UDP or a Multicast IPv6 UDP.
*
* INPUTS:
*       port    - logical port number.
*       type     - policy type (GT_POLICY_TYPE)
*       policy     - policy (GT_FRAME_POLICY)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_BAD_PARAM     - on bad parameters
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetPolicy
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT     port,
    IN  GT_POLICY_TYPE    type,
    IN    GT_FRAME_POLICY    policy
);

/*******************************************************************************
* gprtGetPolicy
*
* DESCRIPTION:
*       This routine gets the Policy of the given policy type.
*        Supported Policies are defined as GT_FRAME_POLICY as follows:
*            FRAME_POLICY_NONE    - normal frame switching
*            FRAME_POLICY_MIRROR  - mirror (copy) frame to MirrorDest port
*            FRAME_POLICY_TRAP    - trap(re-direct) frame to the CPUDest port
*            FRAME_POLICY_DISCARD - discard(filter) the frame
*        Supported Policy types are defined as GT_POLICY_TYPE:
*            POLICY_TYPE_DA - DA Policy Mapping
*                DA Policy Mapping occurs when the DA of a frame is contained in 
*                the ATU address database with an Entry State that indicates Policy.
*            POLICY_TYPE_SA - SA Policy Mapping
*                SA Policy Mapping occurs when the SA of a frame is contained in 
*                the ATU address database with an Entry State that indicates Policy.
*            POLICY_TYPE_VTU - VTU Policy Mapping
*                VTU Policy Mapping occurs when the VID of a frame is contained in
*                the VTU database with the VidPolicy is enabled.
*            POLICY_TYPE_ETYPE - EtherType Policy Mapping
*                EType Policy Mapping occurs when the EtherType of a frame matches
*                the PortEType (see gprtSetPortEType API)
*            POLICY_TYPE_PPPoE - PPPoE Policy Mapping
*                PPPoE Policy Mapping occurs when the EtherType of a frame matches 0x8863
*            POLICY_TYPE_VBAS - VBAS Policy Mapping
*                VBAS Policy Mapping occurs when the EtherType of a frame matches 0x8200
*            POLICY_TYPE_OPT82 - DHCP Option 82 Policy Mapping
*                DHCP Option 82 Policy Mapping occurs when the ingressing frame is an
*                IPv4 UDP with a UDP Destination port = 0x0043 or 0x0044, or an
*                IPv6 UDP with a UDP Destination port = 0x0223 or 0x0222
*            POLICY_TYPE_UDP - UDP Policy Mapping
*                UDP Policy Mapping occurs when the ingressing frame is
*                a Broadcast IPv4 UDP or a Multicast IPv6 UDP.
*
* INPUTS:
*       port    - logical port number.
*       type     - policy type (GT_POLICY_TYPE)
*
* OUTPUTS:
*       policy     - policy (GT_FRAME_POLICY)
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_BAD_PARAM     - on bad parameters
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetPolicy
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT     port,
    IN  GT_POLICY_TYPE    type,
    OUT GT_FRAME_POLICY    *policy
);


/* gtPortCtrl.c */

/*******************************************************************************
* gprtSetPauseLimitOut
*
* DESCRIPTION:
*        Limit the number of continuous Pause refresh frames that can be transmitted
*        from this port. When full duplex Flow Control is enabled on this port,
*        these bits are used to limit the number of Pause refresh frames that can 
*        be generated from this port to keep this port's link partner from sending
*        any data.
*        Setting this value to 0 will allow continuous Pause frame refreshes to 
*        egress this port as long as this port remains congested.
*        Setting this value to 1 will allow 1 Pause frame to egress from this port
*        for each congestion situation.
*        Setting this value to 2 will allow 2 Pause frames to egress from this port
*        for each congestion situation, etc.
*        
* INPUTS:
*        port - the logical port number
*        limit - the max number of Pause refresh frames for each congestion situation
*                ( 0 ~ 0xFF)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if limit > 0xFF
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtSetPauseLimitOut
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    IN  GT_U16        limit
);

/*******************************************************************************
* gprtGetPauseLimitOut
*
* DESCRIPTION:
*        Limit the number of continuous Pause refresh frames that can be transmitted
*        from this port. When full duplex Flow Control is enabled on this port,
*        these bits are used to limit the number of Pause refresh frames that can 
*        be generated from this port to keep this port's link partner from sending
*        any data.
*        Setting this value to 0 will allow continuous Pause frame refreshes to 
*        egress this port as long as this port remains congested.
*        Setting this value to 1 will allow 1 Pause frame to egress from this port
*        for each congestion situation.
*        Setting this value to 2 will allow 2 Pause frames to egress from this port
*        for each congestion situation, etc.
*
* INPUTS:
*        port - the logical port number
*
* OUTPUTS:
*        limit - the max number of Pause refresh frames for each congestion situation
*                ( 0 ~ 0xFF)
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtGetPauseLimitOut
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_U16        *limit
);

/*******************************************************************************
* gprtSetPauseLimitIn
*
* DESCRIPTION:
*        Limit the number of continuous Pause refresh frames that can be received
*        on this port. When a port has flow Control enabled, this value can be 
*        used to limit how long this port can be Paused off to prevent a port stall
*        through jamming.
*        When this value is in the range of 0x01 to 0xFF, and a frame is ready to
*        be transmitted out this port, but it cannot be transmitted due to the port
*        being jammed, this limit mechanism starts. The limit mechanism starts 
*        counting new Pause refresh frames or counts of 16 consecutive collisions.
*        If the counter reaches the value set through this API, the following event
*        will occur:
*            1) Port's ForceFC is enabled,
*            2) Port's FCValue is cleared to a zero, and
*            3) Jam Limit Interrupt is asserted.
*        This effectively disables Flow Control on the port once the Pause timer 
*        expires. If a frame gets transmitted out this port before the counter
*        reaches this limit, then this limit mechanism counter resets back to zero.
*
*        Setting this value to 0 will allow continuous jamming to be received on 
*        this port without the Port's ForceFC and FCValue getting modified.
*
*        The modification of Port's ForceFC and FCValue is the only indication that 
*        the limit was reached on this port.
*        
* INPUTS:
*        port - the logical port number
*        limit - the max number of continuous Pause refresh frames for each trasmition
*                ( 0 ~ 0xFF)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if limit > 0xFF
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtSetPauseLimitIn
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    IN  GT_U16        limit
);

/*******************************************************************************
* gprtGetPauseLimitIn
*
* DESCRIPTION:
*        Limit the number of continuous Pause refresh frames that can be received
*        on this port. When a port has flow Control enabled, this value can be 
*        used to limit how long this port can be Paused off to prevent a port stall
*        through jamming.
*        When this value is in the range of 0x01 to 0xFF, and a frame is ready to
*        be transmitted out this port, but it cannot be transmitted due to the port
*        being jammed, this limit mechanism starts. The limit mechanism starts 
*        counting new Pause refresh frames or counts of 16 consecutive collisions.
*        If the counter reaches the value set through this API, the following event
*        will occur:
*            1) Port's ForceFC is enabled,
*            2) Port's FCValue is cleared to a zero, and
*            3) Jam Limit Interrupt is asserted.
*        This effectively disables Flow Control on the port once the Pause timer 
*        expires. If a frame gets transmitted out this port before the counter
*        reaches this limit, then this limit mechanism counter resets back to zero.
*
*        Setting this value to 0 will allow continuous jamming to be received on 
*        this port without the Port's ForceFC and FCValue getting modified.
*
*        The modification of Port's ForceFC and FCValue is the only indication that 
*        the limit was reached on this port.
*
* INPUTS:
*        port - the logical port number
*
* OUTPUTS:
*        limit - the max number of continuous Pause refresh frames for each trasmition
*                ( 0 ~ 0xFF)
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtGetPauseLimitIn
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_U16        *limit
);

/*******************************************************************************
* gprtSetQtoPause
*
* DESCRIPTION:
*        When IEEE 802.3x flow control is enabled on this port and a properly formed
*		 Pause frame is recieved on this port, this QtoPause value is used to
*		 determine which output queues on this port are Paused off or not.
*        
* INPUTS:
*        port - the logical port number
*        queues - output queues that are paused off
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if mode is unknown
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtSetQtoPause
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_U8		 queues
);

/*******************************************************************************
* gprtGetQtoPause
*
* DESCRIPTION:
*        When IEEE 802.3x flow control is enabled on this port and a properly formed
*		 Pause frame is recieved on this port, this QtoPause value is used to
*		 determine which output queues on this port are Paused off or not.
*        
* INPUTS:
*        port - the logical port number
*
* OUTPUTS:
*        queues - output queues that are paused off
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if mode is unknown
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtGetQtoPause
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_U8		 *queues
);

/*******************************************************************************
* gprtSetRxPFCEn
*
* DESCRIPTION:
*        This rountine sets receive priority flow control enable.
*        
* INPUTS:
*        port - the logical port number
*        pri - which priority that needs to be set
*		 en - GT_TRUE for enable, GT_FALSE for disable
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if mode is unknown
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/

GT_STATUS gprtSetRxPFCEn
(
	IN  GT_QD_DEV		*dev,
    IN  GT_LPORT		port,
    IN  GT_U8			pri,
	IN  GT_BOOL			en
);

/*******************************************************************************
* gprtGetRxPFCEn
*
* DESCRIPTION:
*        This rountine gets receive priority flow control enable.
*        
* INPUTS:
*        port - the logical port number
*        pri - which priority that needs to be set
*
* OUTPUTS:
*		 en - GT_TRUE for enable, GT_FALSE for disable
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if mode is unknown
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtGetRxPFCEn
(
	IN  GT_QD_DEV		*dev,
    IN  GT_LPORT		port,
    IN  GT_U8			pri,
	OUT GT_BOOL			*en
);

/*******************************************************************************
* gprtSetTxPFCEn
*
* DESCRIPTION:
*        This rountine sets transmit priority flow control enable.
*        
* INPUTS:
*        port - the logical port number
*        pri - which priority that needs to be set
*		 en - GT_TRUE for enable, GT_FALSE for disable
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if mode is unknown
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/

GT_STATUS gprtSetTxPFCEn
(
	IN  GT_QD_DEV		*dev,
    IN  GT_LPORT		port,
    IN  GT_U8			pri,
	IN  GT_BOOL			en
);

/*******************************************************************************
* gprtGetTxPFCEn
*
* DESCRIPTION:
*        This rountine gets transmit priority flow control enable.
*        
* INPUTS:
*        port - the logical port number
*        pri - which priority that needs to be set
*
* OUTPUTS:
*		 en - GT_TRUE for enable, GT_FALSE for disable
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if mode is unknown
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtGetTxPFCEn
(
	IN  GT_QD_DEV		*dev,
    IN  GT_LPORT		port,
    IN  GT_U8			pri,
	OUT GT_BOOL			*en
);

/*******************************************************************************
* gprtSetRxPFCtoQ
*
* DESCRIPTION:
*        This rountine sets receive priority flow control frame's Priority X(0~7)
*		 to Egress Queue mapping.
*        
* INPUTS:
*        port - the logical port number
*        pri - which priority that needs to be set
*		 queue - priority queues 
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if mode is unknown
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/

GT_STATUS gprtSetRxPFCtoQ
(
	IN  GT_QD_DEV		*dev,
    IN  GT_LPORT		port,
    IN  GT_U8			pri,
	IN  GT_U8			queue
);

/*******************************************************************************
* gprtGetRxPFCtoQ
*
* DESCRIPTION:
*        This rountine gets receive priority flow control frame's Priority X(0~7)
*		 to Egress Queue mapping.
*        
* INPUTS:
*        port - the logical port number
*        pri - which priority that needs to be set
*
* OUTPUTS:
*		 queue - priority queues 
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if mode is unknown
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/

GT_STATUS gprtGetRxPFCtoQ
(
	IN  GT_QD_DEV		*dev,
    IN  GT_LPORT		port,
    IN  GT_U8			pri,
	OUT GT_U8			*queue
);

/*******************************************************************************
* gprtSetTxPFCtoQ
*
* DESCRIPTION:
*        This rountine sets transmit priority flow control frame's Priority X(0~7)
*		 to Egress Queue mapping.
*        
* INPUTS:
*        port - the logical port number
*        pri - which priority that needs to be set
*		 queue - priority queues 
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if mode is unknown
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/

GT_STATUS gprtSetTxPFCtoQ
(
	IN  GT_QD_DEV		*dev,
    IN  GT_LPORT		port,
    IN  GT_U8			pri,
	IN  GT_U8			queue
);

/*******************************************************************************
* gprtGetTxPFCtoQ
*
* DESCRIPTION:
*        This rountine gets transmit priority flow control frame's Priority X(0~7)
*		 to Egress Queue mapping.
*        
* INPUTS:
*        port - the logical port number
*        pri - which priority that needs to be set
*
* OUTPUTS:
*		 queue - priority queues 
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if mode is unknown
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/

GT_STATUS gprtGetTxPFCtoQ
(
	IN  GT_QD_DEV		*dev,
    IN  GT_LPORT		port,
    IN  GT_U8			pri,
	OUT GT_U8			*queue
);

/*******************************************************************************
* gprtSetFrameMode
*
* DESCRIPTION:
*        Frmae Mode is used to define the expected Ingress and the generated Egress
*        tagging frame format for this port as follows:
*            GT_FRAME_MODE_NORMAL -
*                Normal Network mode uses industry standard IEEE 802.3ac Tagged or 
*                Untagged frames. Tagged frames use an Ether Type of 0x8100.
*            GT_FRAME_MODE_DSA -
*                DSA mode uses a Marvell defined tagged frame format for 
*                Chip-to-Chip and Chip-to-CPU connections.
*            GT_FRAME_MODE_PROVIDER -
*                Provider mode uses user definable Ether Types per port 
*                (see gprtSetPortEType/gprtGetPortEType API).
*            GT_FRAME_MODE_ETHER_TYPE_DSA -
*                Ether Type DSA mode uses standard Marvell DSA Tagged frame info 
*                flowing a user definable Ether Type. This mode allows the mixture
*                of Normal Network frames with DSA Tagged frames and is useful to 
*                be used on ports that connect to a CPU.
*        
* INPUTS:
*        port - the logical port number
*        mode - GT_FRAME_MODE type
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if mode is unknown
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtSetFrameMode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    IN  GT_FRAME_MODE    mode
);

/*******************************************************************************
* gprtGetFrameMode
*
* DESCRIPTION:
*        Frmae Mode is used to define the expected Ingress and the generated Egress
*        tagging frame format for this port as follows:
*            GT_FRAME_MODE_NORMAL -
*                Normal Network mode uses industry standard IEEE 802.3ac Tagged or 
*                Untagged frames. Tagged frames use an Ether Type of 0x8100.
*            GT_FRAME_MODE_DSA -
*                DSA mode uses a Marvell defined tagged frame format for 
*                Chip-to-Chip and Chip-to-CPU connections.
*            GT_FRAME_MODE_PROVIDER -
*                Provider mode uses user definable Ether Types per port 
*                (see gprtSetPortEType/gprtGetPortEType API).
*            GT_FRAME_MODE_ETHER_TYPE_DSA -
*                Ether Type DSA mode uses standard Marvell DSA Tagged frame info 
*                flowing a user definable Ether Type. This mode allows the mixture
*                of Normal Network frames with DSA Tagged frames and is useful to 
*                be used on ports that connect to a CPU.
*
* INPUTS:
*        port - the logical port number
*
* OUTPUTS:
*        mode - GT_FRAME_MODE type
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtGetFrameMode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_FRAME_MODE    *mode
);

/*******************************************************************************
* gprtSetHoldAt1
*
* DESCRIPTION:
*        Hold Aging ATU Entries at an Entry State value of 1. When this feature
*        is set to GT_TRUE, ATU entries associated with this port will age down 
*        to an Entry State of 0x1, but will not go to 0x0 (0x0 would purge the
*        entry)
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE to hold aging ATU entry with Entry State of 1, 
*               GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtSetHoldAt1
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL        mode
);

/*******************************************************************************
* gprtGetHoldAt1
*
* DESCRIPTION:
*        Hold Aging ATU Entries at an Entry State value of 1. When this feature
*        is set to GT_TRUE, ATU entries associated with this port will age down 
*        to an Entry State of 0x1, but will not go to 0x0 (0x0 would purge the
*        entry)
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE to hold aging ATU entry with Entry State of 1, 
*               GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gprtGetHoldAt1
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL      *mode
);


/*******************************************************************************
* gprtSetIntOnAgeOut
*
* DESCRIPTION:
*        Interrupt on Age Out. When aging is enabled, all non-static address 
*        entries in the ATU's address database are periodically aged.
*        When this feature is set to GT_TRUE and an entry associated with this 
*        port is aged out, an AgeOutViolation will be captured for that entry.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE to enable AgeOutViloation interrupt
*               GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtSetIntOnAgeOut
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL        mode
);

/*******************************************************************************
* gprtGetIntOnAgeOut
*
* DESCRIPTION:
*        Interrupt on Age Out. When aging is enabled, all non-static address 
*        entries in the ATU's address database are periodically aged.
*        When this feature is set to GT_TRUE and an entry associated with this 
*        port is aged out, an AgeOutViolation will be captured for that entry.
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE to enable AgeOutViloation interrupt
*               GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gprtGetIntOnAgeOut
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL      *mode
);

/*******************************************************************************
* gprtSetRefreshLocked
*
* DESCRIPTION:
*        Auto Refresh known addresses when port is Locked. Already known addresses 
*        will be auto refreshed when this feature is enabled. When this feature
*        is disabled, auto refreshing will not occur on Locked ports.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE to enable Auto Refresh known addresses on locked port
*               GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtSetRefreshLocked
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL        mode
);

/*******************************************************************************
* gprtGetRefreshLocked
*
* DESCRIPTION:
*        Auto Refresh known addresses when port is Locked. Already known addresses 
*        will be auto refreshed when this feature is enabled. When this feature
*        is disabled, auto refreshing will not occur on Locked ports.
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE to enable Auto Refresh known addresses on locked port
*               GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gprtGetRefreshLocked
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL      *mode
);

/*******************************************************************************
* gprtSetPortEType
*
* DESCRIPTION:
*        This routine sets the port's special Ether Type. This Ether Type is used
*        for Policy (see gprtSetPolicy API) and FrameMode (see gprtSetFrameMode API).
*
* INPUTS:
*        port  - the logical port number
*        etype - port's special ether type
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtSetPortEType
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port,
    IN GT_ETYPE        etype
);

/*******************************************************************************
* gprtGetPortEType
*
* DESCRIPTION:
*        This routine retrieves the port's special Ether Type. This Ether Type is used
*        for Policy (see gprtSetPolicy API) and FrameMode (see gprtSetFrameMode API).
*
* INPUTS:
*        port  - the logical port number
*
* OUTPUTS:
*        etype - port's special ether type
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtGetPortEType
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_ETYPE    *etype
);

/*******************************************************************************
* gprtSetAllowVid0
*
* DESCRIPTION:
*        Allow VID of Zero.
*
* INPUTS:
*        port - the logical port number.
*        allowed - GT_TRUE, Allow frames received with a VID of 0x000 
*                  will NOT be updated with the port�s Default VID.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetAllowVid0
(
    IN GT_QD_DEV       *dev,
    IN GT_LPORT        port,
    IN GT_BOOL         allowed
);

/*******************************************************************************
* gprtGetAllowVid0
*
* DESCRIPTION:
*        This routine gets Ignore FCS setup
*
* DESCRIPTION:
*        Allow VID of Zero.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        allowed - GT_TRUE, Allow frames received with a VID of 0x000 
*                  will NOT be updated with the port�s Default VID.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetAllowVid0
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *allowed
);


/* gtPortRateCtr.c */

/*******************************************************************************
* grcSetFrameOverhead
*
* DESCRIPTION:
*       Egress rate frame overhead adjustment.
*        This field is used to adjust the number of bytes that need to be added to a
*        frame's IFG on a per frame basis.
*
*        The egress rate limiter multiplies the value programmed in this field by four
*        for computing the frame byte offset adjustment value (i.e., the amount the
*        IPG is increased for every frame). This adjustment, if enabled, is made to
*        every egressing frame's IPG and it is made in addition to any other IPG
*        adjustments due to other Egress Rate Control settings.
*
*        The egress overhead adjustment can add the following number of byte times
*        to each frame's IPG: 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52,
*        56 and 60.
*
*        Example:
*        If FrameOverhead = 11, the egress rate limiter would increase the IPG
*        between every frame by an additional 44 bytes.
*
*        Note: When the Count Mode (port offset 0x0A) is in Frame based egress rate
*        shaping mode, these Frame Overhead bits must be 0x0.
*
* INPUTS:
*       port     - logical port number.
*       overhead - Frame overhead (0 ~ 15)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_BAD_PARAM        - on bad parameters
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS grcSetFrameOverhead
(
    IN GT_QD_DEV        *dev,
    IN GT_LPORT            port,
    IN GT_32            overhead
);

/*******************************************************************************
* grcGetFrameOverhead
*
* DESCRIPTION:
*       Egress rate frame overhead adjustment.
*        This field is used to adjust the number of bytes that need to be added to a
*        frame's IFG on a per frame basis.
*
*        The egress rate limiter multiplies the value programmed in this field by four
*        for computing the frame byte offset adjustment value (i.e., the amount the
*        IPG is increased for every frame). This adjustment, if enabled, is made to
*        every egressing frame's IPG and it is made in addition to any other IPG
*        adjustments due to other Egress Rate Control settings.
*
*        The egress overhead adjustment can add the following number of byte times
*        to each frame's IPG: 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52,
*        56 and 60.
*
*        Example:
*        If FrameOverhead = 11, the egress rate limiter would increase the IPG
*        between every frame by an additional 44 bytes.
*
*        Note: When the Count Mode (port offset 0x0A) is in Frame based egress rate
*        shaping mode, these Frame Overhead bits must be 0x0.
*
* INPUTS:
*       port    - logical port number.
*
* OUTPUTS:
*       overhead - Frame overhead (0 ~ 15)
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
*******************************************************************************/
GT_STATUS grcGetFrameOverhead
(
    IN GT_QD_DEV *dev,
    IN  GT_LPORT port,
    OUT GT_32    *overhead
);


/* gtPortStatus.c */

/*******************************************************************************
* gprtGetBufHigh
*
* DESCRIPTION:
*        Output from QC telling the MAC that it should perform Flow Control.
*
* INPUTS:
*        port - the logical port number
*
* OUTPUTS:
*        bufHigh - GT_TRUE, if Flow control required
*                  GT_FALSE, otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtGetBufHigh
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *bufHigh
);

/*******************************************************************************
* gprtGetFcEn
*
* DESCRIPTION:
*        Input into the QC telling it that Flow Control is enabled on this port.
*
* INPUTS:
*        port - the logical port number
*
* OUTPUTS:
*        fcEn - GT_TRUE, if Flow control is enabled
*               GT_FALSE, otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtGetFcEn
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *fcEn
);

/*******************************************************************************
* gprtGetRsvSize
*
* DESCRIPTION:
*        This routine gets Ingress reserved queue size counter.
*        This counter reflects the current number of reserved ingress buffers 
*        assigned to this port.
*
* INPUTS:
*        port - the logical port number
*
* OUTPUTS:
*        count - reserved ingress queue size counter value
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtGetRsvSize
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_U16        *count
);


/* gtPriTable.c */

/*******************************************************************************
* gsysSetQPriOverrideTable
*
* DESCRIPTION:
*       Queue Priority Override.
*        When a frame enters a port, its type is determined and the type is used 
*        to access the Queue Priority Table. If the type's qPriEn (in GT_QPRI_TBL_ENTRY
*        structure) is enabled, then the frame's Queue Priority will be overridden
*        with the value written in qPriority (in GT_QPRI_TBL_ENTRY structure).
*        Frame Types supported are:
*            FTYPE_DSA_TO_CPU_BPDU -
*                Used on multicast DSA To_CPU frames with a Code of 0x0 (BPDU/MGMT).
*                Not used on non-DSA Control frames.
*            FTYPE_DSA_TO_CPU_F2R -
*                Used on DSA To_CPU frames with a Code of 0x1 (Frame to Register
*                Reply). Not used on non-DSA Control frames.
*            FTYPE_DSA_TO_CPU_IGMP -
*                Used on DSA To_CPU frames with a Code of 0x2 (IGMP/MLD Trap)
*                and on non-DSA Control frames that are IGMP or MLD trapped
*            FTYPE_DSA_TO_CPU_TRAP -
*                Used on DSA To_CPU frames with a Code of 0x3 (Policy Trap) and
*                on non-DSA Control frames that are Policy Trapped
*            FTYPE_DSA_TO_CPU_ARP -
*                Used on DSA To_CPU frames with a Code of 0x4 (ARP Mirror) and
*                on non-DSA Control frames that are ARP Mirrored (see gprtSetARPtoCPU API).
*            FTYPE_DSA_TO_CPU_MIRROR -
*                Used on DSA To_CPU frames with a Code of 0x5 (Policy Mirror) and
*                on non-DSA Control frames that are Policy Mirrored (see gprtSetPolicy API).
*            FTYPE_DSA_TO_CPU_RESERVED -
*                Used on DSA To_CPU frames with a Code of 0x6 (Reserved). Not
*                used on non-DSA Control frames.
*            FTYPE_DSA_TO_CPU_UCAST_MGMT -
*                Used on unicast DSA To_CPU frames with a Code of 0x0 (unicast
*                MGMT). Not used on non-DSA Control frames.
*            FTYPE_DSA_FROM_CPU -
*                Used on DSA From_CPU frames. Not used on non-DSA Control frame
*            FTYPE_DSA_CROSS_CHIP_FC -
*                Used on DSA Cross Chip Flow Control frames (To_Sniffer Flow
*                Control). Not used on non-DSA Control frames.
*            FTYPE_DSA_CROSS_CHIP_EGRESS_MON -
*                Used on DSA Cross Chip Egress Monitor frames (To_Sniffer Tx).
*                Not used on non-DSA Control frames.
*            FTYPE_DSA_CROSS_CHIP_INGRESS_MON -
*                Used on DSA Cross Chip Ingress Monitor frames (To_Sniffer Rx).
*                Not used on non-DSA Control frames.
*            FTYPE_PORT_ETYPE_MATCH -
*                Used on normal network ports (see gprtSetFrameMode API)
*                on frames whose Ethertype matches the port's PortEType register.
*                Not used on non-DSA Control frames.
*            FTYPE_BCAST_NON_DSA_CONTROL -
*                Used on Non-DSA Control frames that contain a Broadcast
*                destination address. Not used on DSA Control frames.
*
* INPUTS:
*       fType - frame type (GT_PRI_OVERRIDE_FTYPE)
*       entry - Q Priority Override Table entry (GT_QPRI_TBL_ENTRY)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM     - on unknown frame type
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gsysSetQPriOverrideTable
(
    IN  GT_QD_DEV     *dev,
    IN  GT_PRI_OVERRIDE_FTYPE    fType,
    IN  GT_QPRI_TBL_ENTRY    *entry
);

/*******************************************************************************
* gsysGetQPriOverrideTable
*
* DESCRIPTION:
*       Queue Priority Override.
*        When a frame enters a port, its type is determined and the type is used 
*        to access the Queue Priority Table. If the type's qPriEn (in GT_QPRI_TBL_ENTRY
*        structure) is enabled, then the frame's Queue Priority will be overridden
*        with the value written in qPriority (in GT_QPRI_TBL_ENTRY structure).
*        Frame Types supported are:
*            FTYPE_DSA_TO_CPU_BPDU -
*                Used on multicast DSA To_CPU frames with a Code of 0x0 (BPDU/MGMT).
*                Not used on non-DSA Control frames.
*            FTYPE_DSA_TO_CPU_F2R -
*                Used on DSA To_CPU frames with a Code of 0x1 (Frame to Register
*                Reply). Not used on non-DSA Control frames.
*            FTYPE_DSA_TO_CPU_IGMP -
*                Used on DSA To_CPU frames with a Code of 0x2 (IGMP/MLD Trap)
*                and on non-DSA Control frames that are IGMP or MLD trapped
*            FTYPE_DSA_TO_CPU_TRAP -
*                Used on DSA To_CPU frames with a Code of 0x3 (Policy Trap) and
*                on non-DSA Control frames that are Policy Trapped
*            FTYPE_DSA_TO_CPU_ARP -
*                Used on DSA To_CPU frames with a Code of 0x4 (ARP Mirror) and
*                on non-DSA Control frames that are ARP Mirrored (see gprtSetARPtoCPU API).
*            FTYPE_DSA_TO_CPU_MIRROR -
*                Used on DSA To_CPU frames with a Code of 0x5 (Policy Mirror) and
*                on non-DSA Control frames that are Policy Mirrored (see gprtSetPolicy API).
*            FTYPE_DSA_TO_CPU_RESERVED -
*                Used on DSA To_CPU frames with a Code of 0x6 (Reserved). Not
*                used on non-DSA Control frames.
*            FTYPE_DSA_TO_CPU_UCAST_MGMT -
*                Used on unicast DSA To_CPU frames with a Code of 0x0 (unicast
*                MGMT). Not used on non-DSA Control frames.
*            FTYPE_DSA_FROM_CPU -
*                Used on DSA From_CPU frames. Not used on non-DSA Control frame
*            FTYPE_DSA_CROSS_CHIP_FC -
*                Used on DSA Cross Chip Flow Control frames (To_Sniffer Flow
*                Control). Not used on non-DSA Control frames.
*            FTYPE_DSA_CROSS_CHIP_EGRESS_MON -
*                Used on DSA Cross Chip Egress Monitor frames (To_Sniffer Tx).
*                Not used on non-DSA Control frames.
*            FTYPE_DSA_CROSS_CHIP_INGRESS_MON -
*                Used on DSA Cross Chip Ingress Monitor frames (To_Sniffer Rx).
*                Not used on non-DSA Control frames.
*            FTYPE_PORT_ETYPE_MATCH -
*                Used on normal network ports (see gprtSetFrameMode API)
*                on frames whose Ethertype matches the port's PortEType register.
*                Not used on non-DSA Control frames.
*            FTYPE_BCAST_NON_DSA_CONTROL -
*                Used on Non-DSA Control frames that contain a Broadcast
*                destination address. Not used on DSA Control frames.
*
* INPUTS:
*       fType - frame type (GT_PRI_OVERRIDE_FTYPE)
*
* OUTPUTS:
*       entry - Q Priority Override Table entry (GT_QPRI_TBL_ENTRY)
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM     - on unknown frame type
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gsysGetQPriOverrideTable
(
    IN  GT_QD_DEV     *dev,
    IN  GT_PRI_OVERRIDE_FTYPE    fType,
    OUT GT_QPRI_TBL_ENTRY    *entry
);


/* gtSysCtrl.c */

/*******************************************************************************
* gsysSetCPUDest
*
* DESCRIPTION:
*        This routine sets CPU Destination Port. CPU Destination port indicates the
*        port number on this device where the CPU is connected (either directly or
*        indirectly through another Marvell switch device).
*
*        Many modes of frame processing need to know where the CPU is located.
*        These modes are:
*        1. When IGMP/MLD frame is received and Snooping is enabled
*        2. When the port is configured as a DSA port and it receives a To_CPU frame
*        3. When a Rsvd2CPU frame enters the port
*        4. When the port's SA Filtering mode is Drop to CPU
*        5. When any of the port's Policy Options trap the frame to the CPU
*        6. When the ingressing frame is an ARP and ARP mirroring is enabled in the
*           device
*
*        In all cases, except for ARP, the frames that meet the enabled criteria 
*        are mapped to the CPU Destination port, overriding where the frame would 
*        normally go. In the case of ARP, the frame will be mapped normally and it 
*        will also get copied to this port.
*        Frames that filtered or discarded will not be mapped to the CPU Destination 
*        port with the exception of the Rsvd2CPU and DSA Tag cases.
*
*        If CPUDest = 0xF, the remapped frames will be discarded, no ARP mirroring 
*        will occur and ingressing To_CPU frames will be discarded.
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetCPUDest
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port
);

/*******************************************************************************
* gsysGetCPUDest
*
* DESCRIPTION:
*        This routine gets CPU Destination Port. CPU Destination port indicates the
*        port number on this device where the CPU is connected (either directly or
*        indirectly through another Marvell switch device).
*
*        Many modes of frame processing need to know where the CPU is located.
*        These modes are:
*        1. When IGMP/MLD frame is received and Snooping is enabled
*        2. When the port is configured as a DSA port and it receives a To_CPU frame
*        3. When a Rsvd2CPU frame enters the port
*        4. When the port's SA Filtering mode is Drop to CPU
*        5. When any of the port's Policy Options trap the frame to the CPU
*        6. When the ingressing frame is an ARP and ARP mirroring is enabled in the
*           device
*
*        In all cases, except for ARP, the frames that meet the enabled criteria 
*        are mapped to the CPU Destination port, overriding where the frame would 
*        normally go. In the case of ARP, the frame will be mapped normally and it 
*        will also get copied to this port.
*        Frames that filtered or discarded will not be mapped to the CPU Destination 
*        port with the exception of the Rsvd2CPU and DSA Tag cases.
*
*        If CPUDest = 0xF, the remapped frames will be discarded, no ARP mirroring 
*        will occur and ingressing To_CPU frames will be discarded.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        port  - the logical port number.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysGetCPUDest
(
    IN  GT_QD_DEV    *dev,
    OUT GT_LPORT      *port
);

/*******************************************************************************
* gsysSetMirrorDest
*
* DESCRIPTION:
*        This routine sets Mirror Destination Port. Frames that ingress a port 
*        that trigger a policy mirror are mapped (copied) to this port as long as 
*        the frame is not filtered or discarded. 
*        The Mirror Destination port should point to the port that directs these 
*        frames to the CPU that will process these frames. This target port should 
*        be a DSA Tag port so the frames will egress with a To_CPU DSA Tag with a 
*        CPU Code of Policy Mirror.
*        To_CPU DSA Tag frames with a CPU Code of Policy Mirror that ingress a DSA 
*        Tag port will be sent to the port number defined in MirrorDest.
*
*        If MirrorDest = 0xF, Policy Mirroring is disabled and ingressing To_CPU 
*        Policy Mirror frames will be discarded.
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetMirrorDest
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port
);

/*******************************************************************************
* gsysGetMirrorDest
*
* DESCRIPTION:
*        This routine gets Mirror Destination Port. Frames that ingress a port 
*        that trigger a policy mirror are mapped (copied) to this port as long as 
*        the frame is not filtered or discarded. 
*        The Mirror Destination port should point to the port that directs these 
*        frames to the CPU that will process these frames. This target port should 
*        be a DSA Tag port so the frames will egress with a To_CPU DSA Tag with a 
*        CPU Code of Policy Mirror.
*        To_CPU DSA Tag frames with a CPU Code of Policy Mirror that ingress a DSA 
*        Tag port will be sent to the port number defined in MirrorDest.
*
*        If MirrorDest = 0xF, Policy Mirroring is disabled and ingressing To_CPU 
*        Policy Mirror frames will be discarded.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        port  - the logical port number.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysGetMirrorDest
(
    IN  GT_QD_DEV    *dev,
    OUT GT_LPORT      *port
);

/*******************************************************************************
* gsysSetRMPort
*
* DESCRIPTION:
*        Remote Management feature is enabled only on one port. Since not all ports
*        can be enabled for Remote Management feature, please refer to the device
*        datasheet for detailed information.
*        For example, 88E6097 device allows logical port 9 or 10, and 88E6047 
*        device allows logical port 4 and 5.
*        This is old function. For latest switch, it is replaced by gsysSetRMUPort.
*        
* INPUTS:
*        port - Remote Management Port
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM     - on unallowable port
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        Obsolete. Please uses gsysSetRMUMode API, instead.
*
*******************************************************************************/
GT_STATUS gsysSetRMPort
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port
);

/*******************************************************************************
* gsysGetRMPort
*
* DESCRIPTION:
*        Remote Management feature is enabled only on one port. Since not all ports
*        can be enabled for Remote Management feature, please refer to the device
*        datasheet for detailed information.
*        For example, 88E6097 device allows logical port 9 or 10, and 88E6047 
*        device allows logical port 4 and 5.
*        This is old function. For latest switch, it is replaced by gsysGetRMUPort.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        port - Remote Management Port
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        Obsolete. Please uses gsysGetRMUMode API, instead.
*
*******************************************************************************/
GT_STATUS gsysGetRMPort
(
    IN  GT_QD_DEV    *dev,
    OUT GT_LPORT     *port
);

/*******************************************************************************
* gsysSetRMDACheck
*
* DESCRIPTION:
*        Check the DA on Remote Management frames. 
*        When DA Check is enabled, the DA of Remote Management frames must be 
*        contained in this device's address database (ATU) as a Static entry 
*        (either unicast or multicast). If the DA of the frame is not contained 
*        in this device's address database, the frame will be not be processed as 
*        a Frame-to-Regter frame.
*        When DA Check is disabled, the DA of Remote Management frames is not 
*        validated before processing the frame.
*        
* INPUTS:
*        en - GT_TRUE to enable DA Check, 
*             GT_FALSE otherwise.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetRMDACheck
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL         en
);

/*******************************************************************************
* gsysGetRMDACheck
*
* DESCRIPTION:
*        Check the DA on Remote Management frames. 
*        When DA Check is enabled, the DA of Remote Management frames must be 
*        contained in this device's address database (ATU) as a Static entry 
*        (either unicast or multicast). If the DA of the frame is not contained 
*        in this device's address database, the frame will be not be processed as 
*        a Frame-to-Regter frame.
*        When DA Check is disabled, the DA of Remote Management frames is not 
*        validated before processing the frame.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        en - GT_TRUE if DA Check is enabled, 
*             GT_FALSE otherwise.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysGetRMDACheck
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL     *en
);

/*******************************************************************************
* gsysSetHeaderType
*
* DESCRIPTION:
*   To set Header Type. These bits are used to configure the bits that are placed 
*   into the Egress Header when it is enabled on a port (Port offset 0x04) 
*   as follows:
*     00 = Original Header � for backwards compatibility to UniMAC�s that look at
*          Header byte 1 bits[4:2] and byte 2 bits [3:0]
*     01 = Single chip MGMT Header � for compatibility to Marvell Fast Ethernet
*          switches that support Spanning Tree without DSA Tags
*     10 = Trunk Header � used together with the DSA Tags to perform Remote Switching
*     11 = Reserved for future use.
*        
* INPUTS:
*        hdType 
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetHeaderType
(
    IN GT_QD_DEV    *dev,
    IN GT_U16       hdType
);

/*******************************************************************************
* gsysGetHeaderType
*
* DESCRIPTION:
*   To get Header Type. These bits are used to configure the bits that are placed 
*   into the Egress Header when it is enabled on a port (Port offset 0x04) 
*   as follows:
*     00 = Original Header � for backwards compatibility to UniMAC�s that look at
*          Header byte 1 bits[4:2] and byte 2 bits [3:0]
*     01 = Single chip MGMT Header � for compatibility to Marvell Fast Ethernet
*          switches that support Spanning Tree without DSA Tags
*     10 = Trunk Header � used together with the DSA Tags to perform Remote Switching
*     11 = Reserved for future use.
*        
* INPUTS:
*        None.
*
* OUTPUTS:
*        hdType 
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysGetHeaderType
(
    IN  GT_QD_DEV    *dev,
    OUT GT_U16     *hdType
);

/*******************************************************************************
* gsysSetRMEnable
*
* DESCRIPTION:
*        Enable or disable Remote Management feature. This feature can be enabled
*        only on one port (see gsysSetRMPort API).
*        
* INPUTS:
*        en - GT_TRUE to enable Remote Management feature, 
*             GT_FALSE otherwise.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        Obsolete. Please uses gsysSetRMUMode API, instead.
*
*******************************************************************************/
GT_STATUS gsysSetRMEnable
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL         en
);

/*******************************************************************************
* gsysGetRMEnable
*
* DESCRIPTION:
*        Enable or disable Remote Management feature. This feature can be enabled
*        only on one port (see gsysSetRMPort API).
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        en - GT_TRUE if Remote Management feature is enabled, 
*             GT_FALSE otherwise.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        Obsolete. Please uses gsysGetRMUMode API, instead.
*
*******************************************************************************/
GT_STATUS gsysGetRMEnable
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL     *en
);

/*******************************************************************************
* gsysSetCtrMode
*
* DESCRIPTION:
*        Set Counter Modes. These bits control the operating modes of the two of 
*        the Port�s MIB counters.
*        
* INPUTS:
*        ctrMode - Counter mode
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM     - on bad parameter
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetCtrMode
(
    IN GT_QD_DEV    *dev,
    IN GT_U16       ctrMode
);

/*******************************************************************************
* gsysGetCtrMode
*
* DESCRIPTION:
*        Get Counter Modes. These bits control the operating modes of the two of 
*        the Port�s MIB counters.
*        
* INPUTS:
*        None.
*
* OUTPUTS:
*        ctrMode - Counter mode
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysGetCtrMode
(
    IN  GT_QD_DEV    *dev,
    OUT GT_U16       *ctrMode
);

/*******************************************************************************
* gsysSetRsvd2CpuEnables2X
*
* DESCRIPTION:
*        Reserved DA Enables for the form of 01:80:C2:00:00:2x.
*        When the Rsvd2Cpu(gsysSetRsvd2Cpu) is set to a one, the 16 reserved 
*        multicast DA addresses, whose bit in this register are also set to a one, 
*        are treadted as MGMT frames. All the reserved DA's take the form 
*        01:80:C2:00:00:2x. When x = 0x0, bit 0 of this register is tested. 
*        When x = 0x2, bit 2 of this field is tested and so on.
*        If the tested bit in this register is cleared to a zero, the frame will 
*        be treated as a normal (non-MGMT) frame.
*
* INPUTS:
*        enBits - bit vector of enabled Reserved Multicast.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetRsvd2CpuEnables2X
(
    IN GT_QD_DEV    *dev,
    IN GT_U16        enBits
);

/*******************************************************************************
* gsysGetRsvd2CpuEnables2X
*
* DESCRIPTION:
*        Reserved DA Enables for the form of 01:80:C2:00:00:2x.
*        When the Rsvd2Cpu(gsysSetRsvd2Cpu) is set to a one, the 16 reserved 
*        multicast DA addresses, whose bit in this register are also set to a one, 
*        are treadted as MGMT frames. All the reserved DA's take the form 
*        01:80:C2:00:00:2x. When x = 0x0, bit 0 of this register is tested. 
*        When x = 0x2, bit 2 of this field is tested and so on.
*        If the tested bit in this register is cleared to a zero, the frame will 
*        be treated as a normal (non-MGMT) frame.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        enBits - bit vector of enabled Reserved Multicast.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysGetRsvd2CpuEnables2X
(
    IN  GT_QD_DEV    *dev,
    OUT GT_U16      *enBits
);

/*******************************************************************************
* gsysSetLoopbackFilter
*
* DESCRIPTION:
*        Loopback Filter.
*        When Loopback Filter is enabled,Forward DSA frames that ingress a DSA port
*        that came from the same Src_Dev will be filtered to the same Src_Port,
*        i.e., the frame will not be allowed to egress the source port on the 
*        source device as indicated in the DSA Forward's Tag.
*
* INPUTS:
*        en - GT_TRUE to enable LoopbackFilter, GT_FALSE otherwise.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetLoopbackFilter
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL        en
);

/*******************************************************************************
* gsysGetLoopbackFilter
*
* DESCRIPTION:
*        Loopback Filter.
*        When Loopback Filter is enabled,Forward DSA frames that ingress a DSA port
*        that came from the same Src_Dev will be filtered to the same Src_Port,
*        i.e., the frame will not be allowed to egress the source port on the 
*        source device as indicated in the DSA Forward's Tag.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        en - GT_TRUE if LoopbackFilter is enabled, GT_FALSE otherwise.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysGetLoopbackFilter
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL      *en
);

/*******************************************************************************
* gsysSetFloodBC
*
* DESCRIPTION:
*        Flood Broadcast.
*        When Flood Broadcast is enabled, frames with the Broadcast destination 
*        address will flood out all the ports regardless of the setting of the
*        port's Egress Floods mode (see gprtSetEgressFlood API). VLAN rules and 
*        other switch policy still applies to these Broadcast frames. 
*        When this feature is disabled, frames with the Broadcast destination
*        address are considered Multicast frames and will be affected by port's 
*        Egress Floods mode.
*
* INPUTS:
*        en - GT_TRUE to enable Flood Broadcast, GT_FALSE otherwise.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetFloodBC
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL        en
);

/*******************************************************************************
* gsysGetFloodBC
*
* DESCRIPTION:
*        Flood Broadcast.
*        When Flood Broadcast is enabled, frames with the Broadcast destination 
*        address will flood out all the ports regardless of the setting of the
*        port's Egress Floods mode (see gprtSetEgressFlood API). VLAN rules and 
*        other switch policy still applies to these Broadcast frames. 
*        When this feature is disabled, frames with the Broadcast destination
*        address are considered Multicast frames and will be affected by port's 
*        Egress Floods mode.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        en - GT_TRUE if Flood Broadcast is enabled, GT_FALSE otherwise.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysGetFloodBC
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL      *en
);

/*******************************************************************************
* gsysSetRemove1PTag
*
* DESCRIPTION:
*        Remove One Provider Tag.
*        When this feature is enabled and a port is configured as a Provider Port
*        (see gprtSetFrameMode API), recursive Provider Tag stripping will NOT be 
*        performed. Only the first Provider Tag found on the frame will be 
*        extracted and removed. Its extracted data will be used for switching.
*        When it's disabled and a port is configured as a Provider Port, recursive 
*        Provider Tag stripping will be performed. The first Provider Tag's data 
*        will be extracted and used for switching, and then all subsequent Provider 
*        Tags found in the frame will also be removed. This will only occur if the 
*        port's PortEType (see gprtSetPortEType API) is not 0x8100.
*
* INPUTS:
*        en - GT_TRUE to enable Remove One Provider Tag, GT_FALSE otherwise.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetRemove1PTag
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL        en
);

/*******************************************************************************
* gsysGetRemove1PTag
*
* DESCRIPTION:
*        Remove One Provider Tag.
*        When this feature is enabled and a port is configured as a Provider Port
*        (see gprtSetFrameMode API), recursive Provider Tag stripping will NOT be 
*        performed. Only the first Provider Tag found on the frame will be 
*        extracted and removed. Its extracted data will be used for switching.
*        When it's disabled and a port is configured as a Provider Port, recursive 
*        Provider Tag stripping will be performed. The first Provider Tag's data 
*        will be extracted and used for switching, and then all subsequent Provider 
*        Tags found in the frame will also be removed. This will only occur if the 
*        port's PortEType (see gprtSetPortEType API) is not 0x8100.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        en - GT_TRUE if Remove One Provider Tag is enabled, GT_FALSE otherwise.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysGetRemove1PTag
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL        *en
);

/*******************************************************************************
* gsysSetTagFlowControl
*
* DESCRIPTION:
*        Use and generate source port Flow Control status for Cross-Chip Flow 
*        Control.
*        When this feature is enabled, bit 17 of the DSA Tag Forward frames is 
*        defined to be Src_FC and it is added to these frames when generated and 
*        it is inspected on these frames when received. The QC will use the Src_FC 
*        bit on DSA ports instead of the DSA port's Flow Control mode bit for the 
*        QC Flow Control algorithm.
*        When it is disabled, bit 17 of the DSA Tag Forward frames is defined to 
*        be Reserved and it will be zero on these frames when generated and it 
*        will not be used on these frames when received (this is a backwards 
*        compatibility mode).
*
* INPUTS:
*        en - GT_TRUE to enable Tag Flow Control, GT_FALSE otherwise.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetTagFlowControl
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL        en
);

/*******************************************************************************
* gsysGetTagFlowControl
*
* DESCRIPTION:
*        Use and generate source port Flow Control status for Cross-Chip Flow 
*        Control.
*        When this feature is enabled, bit 17 of the DSA Tag Forward frames is 
*        defined to be Src_FC and it is added to these frames when generated and 
*        it is inspected on these frames when received. The QC will use the Src_FC 
*        bit on DSA ports instead of the DSA port's Flow Control mode bit for the 
*        QC Flow Control algorithm.
*        When it is disabled, bit 17 of the DSA Tag Forward frames is defined to 
*        be Reserved and it will be zero on these frames when generated and it 
*        will not be used on these frames when received (this is a backwards 
*        compatibility mode).
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        en - GT_TRUE if Tag Flow Control is enabled, GT_FALSE otherwise.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysGetTagFlowControl
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL      *en
);

/*******************************************************************************
* gsysSetAlwaysUseVTU
*
* DESCRIPTION:
*        Always use VTU. 
*        When this feature is enabled, VTU hit data will be used to map frames 
*        even if 802.1Q is Disabled on the port. 
*        When it's disabled, data will be ignored when mapping frames on ports 
*        where 802.1Q is Disabled.
*
* INPUTS:
*        en - GT_TRUE to use VTU always, GT_FALSE otherwise.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetAlwaysUseVTU
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL        en
);

/*******************************************************************************
* gsysGetAlwaysUseVTU
*
* DESCRIPTION:
*        Always use VTU. 
*        When this feature is enabled, VTU hit data will be used to map frames 
*        even if 802.1Q is Disabled on the port. 
*        When it's disabled, data will be ignored when mapping frames on ports 
*        where 802.1Q is Disabled.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        en - GT_TRUE if VTU is always used, GT_FALSE otherwise.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysGetAlwaysUseVTU
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL      *en
);

/*******************************************************************************
* gsysSetQVlansOnly
*
* DESCRIPTION:
*        802.1Q VLANs Only.
*        When this feature is disabled, the egress mapping of the frame is 
*        limited by the frame's VID (using the MemberTag data found in the VTU) 
*        together with the port based VLANs (using the source port's PortVLANTable, 
*        gvlnSetPortVlanPorts API). The two methods are always used together in 
*        this mode.
*        When this feature is enabled, the egress mapping of the frame is limitied
*        by the frame's VID only, if the VID was found in the VTU. If the frame's
*        VID was not found in the VTU the egress mapping of the frame is limited
*        by the source port's PortVLANTable only. The two methods are never
*        used together in this mode.
*
* INPUTS:
*        en - GT_TRUE to use 802.1Q Vlan Only feature, GT_FALSE otherwise.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetQVlansOnly
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL        en
);

/*******************************************************************************
* gsysGetQVlansOnly
*
* DESCRIPTION:
*        802.1Q VLANs Only.
*        When this feature is disabled, the egress mapping of the frame is 
*        limited by the frame's VID (using the MemberTag data found in the VTU) 
*        together with the port based VLANs (using the source port's PortVLANTable, 
*        gvlnSetPortVlanPorts API). The two methods are always used together in 
*        this mode.
*        When this feature is enabled, the egress mapping of the frame is limitied
*        by the frame's VID only, if the VID was found in the VTU. If the frame's
*        VID was not found in the VTU the egress mapping of the frame is limited
*        by the source port's PortVLANTable only. The two methods are never
*        used together in this mode.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        en - GT_TRUE if 802.1Q Vlan Only feature is enabled, GT_FALSE otherwise.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysGetQVlansOnly
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL      *en
);

/*******************************************************************************
* gsysSet5BitPort
*
* DESCRIPTION:
*        Use 5 bits for Port data in the Port VLAN Table (PVT). 
*        When this feature is enabled, the 9 bits used to access the PVT memory is:
*            Addr[8:5] = Source Device[3:0] or Device Number[3:0]
*            Addr[4:0] = Source Port/Trunk[4:0]
*        When it's disabled, the 9 bits used to access the PVT memory is:
*            Addr[8:4] = Source Device[4:0] or Device Number[4:0]
*            Addr[3:0] = Source Port/Trunk[3:0]
*
* INPUTS:
*        en - GT_TRUE to use 5 bit as a Source port in PVT, GT_FALSE otherwise.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSet5BitPort
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL        en
);

/*******************************************************************************
* gsysGet5BitPort
*
* DESCRIPTION:
*        Use 5 bits for Port data in the Port VLAN Table (PVT). 
*        When this feature is enabled, the 9 bits used to access the PVT memory is:
*            Addr[8:5] = Source Device[3:0] or Device Number[3:0]
*            Addr[4:0] = Source Port/Trunk[4:0]
*        When it's disabled, the 9 bits used to access the PVT memory is:
*            Addr[8:4] = Source Device[4:0] or Device Number[4:0]
*            Addr[3:0] = Source Port/Trunk[3:0]
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        en - GT_TRUE if 5 bit is used as a Source Port in PVT, GT_FALSE otherwise.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysGet5BitPort
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL      *en
);

/*******************************************************************************
* gsysSetSDETPolarity
*
* DESCRIPTION:
*        SDET (Signal Detect) Polarity select bits for each port. 
*        Bit 10 is for Port 10, bit 9 is for Port 9, etc. SDET is used to help 
*        determine link on fiber ports. This bit affects the active level of a 
*        port's SDET pins as follows:
*            0 = SDET is active low. A low level on the port's SDET pin is 
*                required for link to occur.
*            1 = SDET is active high. A high level on the port�s SDET pin is 
*                required for link to occur.
*        SDET is used when the port is configured as a fiber port. In all other 
*        port modes the SDET pins are ignored and these bits have no effect.
*
* INPUTS:
*        sdetVec - SDET Polarity for each port in Vector format
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if sdetVec is invalid
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetSDETPolarity
(
    IN GT_QD_DEV    *dev,
    IN GT_U32          sdetVec
);

/*******************************************************************************
* gsysSetSDETPolarity
*
* DESCRIPTION:
*        SDET (Signal Detect) Polarity select bits for each port. 
*        Bit 10 is for Port 10, bit 9 is for Port 9, etc. SDET is used to help 
*        determine link on fiber ports. This bit affects the active level of a 
*        port's SDET pins as follows:
*            0 = SDET is active low. A low level on the port's SDET pin is 
*                required for link to occur.
*            1 = SDET is active high. A high level on the port�s SDET pin is 
*                required for link to occur.
*        SDET is used when the port is configured as a fiber port. In all other 
*        port modes the SDET pins are ignored and these bits have no effect.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        sdetVec - SDET Polarity for each port in Vector format
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysGetSDETPolarity
(
    IN  GT_QD_DEV    *dev,
    OUT GT_U32      *sdetVec
);


/* gtBrgVlan.c for 2.6 release */

/*******************************************************************************
* gvlnSetNoEgrPolicy
*
* DESCRIPTION:
*        No Egress Policy. When this bit is set to a one Egress 802.1Q Secure and
*        Check discards are not performed. This mode allowsa non-802.1Q enabled 
*        port to send a frame to an 802.1Q enabled port that is configured in the
*        Secure or Check 802.1Q mode. In this situation the frames will egress 
*        even if the VID assigned to the frame is not found in the VTU.
*
* INPUTS:
*        mode - no egress policy mode
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gvlnSetNoEgrPolicy
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL        mode
);


/*******************************************************************************
* gvlnGetNoEgrPolicy
*
* DESCRIPTION:
*        No Egress Policy. When this bit is set to a one Egress 802.1Q Secure and
*        Check discards are not performed. This mode allowsa non-802.1Q enabled 
*        port to send a frame to an 802.1Q enabled port that is configured in the
*        Secure or Check 802.1Q mode. In this situation the frames will egress 
*        even if the VID assigned to the frame is not found in the VTU.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        mode - no egress policy mode
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gvlnGetNoEgrPolicy
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL        *mode
);

/*******************************************************************************
* gwdSetRMUTimeOut
*
* DESCRIPTION:
*        Remote Management Timeout. When this bit is set to a one the Remote
*        Management Unit(RMU) will timeout on Wait on Bit commands. If the bit that
*        is being tested has not gone to the specified value after 1 sec. has elapsed
*        the Wait on Bit command will be terminated and the Response frame will be
*        sent without any further processing.
*
*        When this bit is cleared to a zero the Wait on Bit command will wait 
*        until the bit that is being tested has changed to the specified value.
*
* INPUTS:
*        en   - GT_TRUE to enable RMU Timeout
*               GT_FALUSE to disable
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gwdSetRMUTimeOut
(
    IN  GT_QD_DEV    *dev,
    IN  GT_BOOL        en
);

/*******************************************************************************
* gwdGetRMUTimeOut
*
* DESCRIPTION:
*        Remote Management Timeout. When this bit is set to a one the Remote
*        Management Unit(RMU) will timeout on Wait on Bit commands. If the bit that
*        is being tested has not gone to the specified value after 1 sec. has elapsed
*        the Wait on Bit command will be terminated and the Response frame will be
*        sent without any further processing.
*
*        When this bit is cleared to a zero the Wait on Bit command will wait 
*        until the bit that is being tested has changed to the specified value.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        en   - GT_TRUE to enable RMU Timeout
*               GT_FALUSE, otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gwdGetRMUTimeOut
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL        *en
);

/*******************************************************************************
* gwdGetEgressWDEvent
*
* DESCRIPTION:
*        If any port's egress logic detects an egress watch dog issue, this bit
*        will be set to a one, regardless of the setting of the GT_WD_EGRESS in
*        gwdSetEvent function.
*        
* INPUTS:
*        None.
*
* OUTPUTS:
*        event - GT_TRUE, if egress logic has detected any egress watch dog issue
*                GT_FALUSE, otherwise
*
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gwdGetEgressWDEvent
(
    IN  GT_QD_DEV        *dev,
    OUT GT_BOOL            *event
);


/*******************************************************************************
* gsysSetQoSWeight
*
* DESCRIPTION:
*       Programmable Round Robin Weights.
*        Each port has 4 output Queues. Queue 3 has the highest priority and 
*        Queue 0 has the lowest priority. When a scheduling mode of port is 
*        configured as Weighted Round Robin queuing mode, the access sequece of the 
*        Queue is 3,2,3,1,3,2,3,0,3,2,3,1,3,2,3 by default.
*        This sequence can be configured with this API.
*
* INPUTS:
*       weight - access sequence of the queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gsysSetQoSWeight
(
    IN  GT_QD_DEV         *dev,
    IN  GT_QoS_WEIGHT    *weight
);

/*******************************************************************************
* gsysGetQoSWeight
*
* DESCRIPTION:
*       Programmable Round Robin Weights.
*        Each port has 4 output Queues. Queue 3 has the highest priority and 
*        Queue 0 has the lowest priority. When a scheduling mode of port is 
*        configured as Weighted Round Robin queuing mode, the access sequece of the 
*        Queue is 3,2,3,1,3,2,3,0,3,2,3,1,3,2,3 by default.
*        This routine retrieves the access sequence of the Queue.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       weight - access sequence of the queue
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gsysGetQoSWeight
(
    IN  GT_QD_DEV         *dev,
    OUT GT_QoS_WEIGHT    *weight
);


/*
 *    gtPortCtrl.c
*/

/*******************************************************************************
* gsysSetJumboMode
*
* DESCRIPTION:
*       This routine Set the max frame size allowed to be received and transmitted
*        from or to a given port.
*
* INPUTS:
*        port - the logical port number
*       mode - GT_JUMBO_MODE (1522, 2048, or 10240)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gsysSetJumboMode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    IN  GT_JUMBO_MODE   mode
);

/*******************************************************************************
* gsysGetJumboMode
*
* DESCRIPTION:
*       This routine gets the max frame size allowed to be received and transmitted
*        from or to a given port.
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*       mode - GT_JUMBO_MODE (1522, 2048, or 10240)
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gsysGetJumboMode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_JUMBO_MODE   *mode
);

/*
 *  gtPhyCtrl.c
*/
/*******************************************************************************
* gprtGetEnergyDetect
*
* DESCRIPTION:
*       Energy Detect power down mode enables or disables the PHY to wake up on
*        its own by detecting activity on the CAT 5 cable. 
*
* INPUTS:
*         port - The logical port number
*
* OUTPUTS:
*       mode - GT_EDETECT_MODE type
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gprtGetEnergyDetect
(
    IN  GT_QD_DEV *dev,
    IN  GT_LPORT  port,
    OUT GT_EDETECT_MODE   *mode
);

/*******************************************************************************
* gprtSetEnergyDetect
*
* DESCRIPTION:
*       Energy Detect power down mode enables or disables the PHY to wake up on
*        its own by detecting activity on the CAT 5 cable. 
*
* INPUTS:
*         port - The logical port number
*       mode - GT_EDETECT_MODE type
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/

GT_STATUS gprtSetEnergyDetect
(
    IN  GT_QD_DEV *dev,
    IN  GT_LPORT  port,
    IN  GT_EDETECT_MODE   mode
);


/*
 *    gtSysCtrl.c
*/

/*******************************************************************************
* gsysSetRMUMode
*
* DESCRIPTION:
*        Set Rmote Management Unit Mode: disable, enable on port 4, 5 or 6, or enable
*        on port 9 or 10. Devices, such as 88E6097, support RMU on port 9 and 10, 
*        while other devices, such as 88E6165, support RMU on port 4, 5 and 6. So,
*        please refer to the device datasheet for detail.
*        When RMU is enabled and this device receives a Remote Management Request
*        frame directed to this device, the frame will be processed and a Remote 
*        Management Response frame will be generated and sent out.
*        
*        Note: enabling RMU has no effect if the Remote Management port is in half
*        duplex mode. The port's FrameMode must be DSA or EtherType DSA as well.
*        
* INPUTS:
*        rmu - GT_RMU structure
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM     - on bad parameter
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetRMUMode
(
    IN GT_QD_DEV    *dev,
    IN GT_RMU        *rmu
);

/*******************************************************************************
* gsysGetRMUMode
*
* DESCRIPTION:
*        Get Rmote Management Unit Mode: disable, enable on port 4, 5 or 6, or enable
*        on port 9 or 10. Devices, such as 88E6097, support RMU on port 9 and 10, 
*        while other devices, such as 88E6165, support RMU on port 4, 5 and 6. So,
*        please refer to the device datasheet for detail.
*        When RMU is enabled and this device receives a Remote Management Request
*        frame directed to this device, the frame will be processed and a Remote 
*        Management Response frame will be generated and sent out.
*        
*        Note: enabling RMU has no effect if the Remote Management port is in half
*        duplex mode. The port's FrameMode must be DSA or EtherType DSA as well.
*        
* INPUTS:
*        None.
*
* OUTPUTS:
*        rmu - GT_RMU structure
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysGetRMUMode
(
    IN  GT_QD_DEV    *dev,
    OUT GT_RMU        *rmu
);

/*******************************************************************************
* gsysPort2Lport
*
* DESCRIPTION:
*        This routine converts physical port number to logical port number.
*
* INPUTS:
*        port - physical port number
*
* OUTPUTS:
*        lport - logical port number
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysPort2Lport
(
    IN  GT_QD_DEV    *dev,
    IN  GT_U32         port,
    OUT GT_LPORT    *lport
);

/*******************************************************************************
* gsysLport2Port
*
* DESCRIPTION:
*        This routine converts logical port number to physical port number.
*
* INPUTS:
*        lport - logical port number
*
* OUTPUTS:
*        port - physical port number
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysLport2Port
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    lport,
    OUT GT_U32         *port
);

/*******************************************************************************
* gsysPortvec2Lportvec
*
* DESCRIPTION:
*        This routine converts physical port vector to logical port vector.
*
* INPUTS:
*        portvec - physical port vector
*
* OUTPUTS:
*        lportvec - logical port vector
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysPortvec2Lportvec
(
    IN  GT_QD_DEV    *dev,
    IN  GT_U32        portvec,
    OUT GT_U32         *lportvec
);

/*******************************************************************************
* gsysLportvec2Portvec
*
* DESCRIPTION:
*        This routine converts logical port vector to physical port vector.
*
* INPUTS:
*        lportvec - logical port vector
*
* OUTPUTS:
*        portvec - physical port vector
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysLportvec2Portvec
(
    IN  GT_QD_DEV    *dev,
    IN  GT_U32        lportvec,
    OUT GT_U32         *portvec
);



/*
 * gtPTP.c
 */

/*******************************************************************************
* gptpSetConfig
*
* DESCRIPTION:
*       This routine writes PTP configuration parameters.
*
* INPUTS:
*        ptpData  - PTP configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpSetConfig
(
    IN  GT_QD_DEV     *dev,
    IN  GT_PTP_CONFIG    *ptpData
);

/*******************************************************************************
* gptpGetConfig
*
* DESCRIPTION:
*       This routine reads PTP configuration parameters.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        ptpData  - PTP configuration parameters.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetConfig
(
    IN  GT_QD_DEV     *dev,
    OUT GT_PTP_CONFIG    *ptpData
);

/*******************************************************************************
* gptpSetGlobalConfig
*
* DESCRIPTION:
*       This routine writes PTP global configuration parameters.
*
* INPUTS:
*        ptpData  - PTP global configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpSetGlobalConfig
(
    IN  GT_QD_DEV     *dev,
    IN  GT_PTP_GLOBAL_CONFIG    *ptpData
);

/*******************************************************************************
* gptpGetGlobalConfig
*
* DESCRIPTION:
*       This routine reads PTP global configuration parameters.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        ptpData  - PTP global configuration parameters.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetGlobalConfig
(
    IN  GT_QD_DEV     *dev,
    OUT GT_PTP_GLOBAL_CONFIG    *ptpData
);

/*******************************************************************************
* gptpSetPortConfig
*
* DESCRIPTION:
*       This routine writes PTP port configuration parameters.
*
* INPUTS:
*        ptpData  - PTP port configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpSetPortConfig
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    IN  GT_PTP_PORT_CONFIG    *ptpData
);

/*******************************************************************************
* gptpGetPortConfig
*
* DESCRIPTION:
*       This routine reads PTP configuration parameters for a port.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        ptpData  - PTP port configuration parameters.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetPortConfig
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    OUT GT_PTP_PORT_CONFIG    *ptpData
);

/*******************************************************************************
* gptpSetPTPEn
*
* DESCRIPTION:
*       This routine enables or disables PTP.
*
* INPUTS:
*        en - GT_TRUE to enable PTP, GT_FALSE to disable PTP
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpSetPTPEn
(
    IN  GT_QD_DEV     *dev,
    IN  GT_BOOL        en
);

/*******************************************************************************
* gptpGetPTPEn
*
* DESCRIPTION:
*       This routine checks if PTP is enabled.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        en - GT_TRUE if enabled, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetPTPEn
(
    IN  GT_QD_DEV     *dev,
    OUT GT_BOOL        *en
);

/*******************************************************************************
* gptpSetPortTsMode
*
* DESCRIPTION:
*       This routine set PTP arrive 0 TS mode on a port.
*
* INPUTS:
*        tsMod - GT_PTP_TS_MODE_IN_REG
*                GT_PTP_TS_MODE_IN_RESERVED_2
*                GT_PTP_TS_MODE_IN_FRAME_END
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpSetPortTsMode
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    IN  GT_PTP_TS_MODE  tsMode 
);

/*******************************************************************************
* gptpGetPortTsMode
*
* DESCRIPTION:
*       This routine get PTP arrive 0 TS mode on a port.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        tsMod - GT_PTP_TS_MODE_IN_REG
*                GT_PTP_TS_MODE_IN_RESERVED_2
*                GT_PTP_TS_MODE_IN_FRAME_END
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetPortTsMode
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    OUT GT_PTP_TS_MODE  *tsMode 
);

/*******************************************************************************
* gptpSetPortPTPEn
*
* DESCRIPTION:
*       This routine enables or disables PTP on a port.
*
* INPUTS:
*        en - GT_TRUE to enable PTP, GT_FALSE to disable PTP
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpSetPortPTPEn
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    IN  GT_BOOL        en
);

/*******************************************************************************
* gptpGetPortPTPEn
*
* DESCRIPTION:
*       This routine checks if PTP is enabled on a port.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        en - GT_TRUE if enabled, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetPortPTPEn
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    OUT GT_BOOL        *en
);


/*******************************************************************************
* gptpGetPTPInt
*
* DESCRIPTION:
*       This routine gets PTP interrupt status for each port.
*        The PTP Interrupt bit gets set for a given port when an incoming PTP 
*        frame is time stamped and PTPArrIntEn for that port is set to 0x1.
*        Similary PTP Interrupt bit gets set for a given port when an outgoing
*        PTP frame is time stamped and PTPDepIntEn for that port is set to 0x1.
*        This bit gets cleared upon software reading and clearing the corresponding
*        time counter valid bits that are valid for that port.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        ptpInt     - interrupt status for each port (bit 0 for port 0, bit 1 for port 1, etc.)
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetPTPInt
(
    IN  GT_QD_DEV     *dev,
    OUT GT_U32        *ptpInt
);

/*******************************************************************************
* gptpGetPTPIntStatus
*
* DESCRIPTION:
*       This routine gets PTP interrupt status for global and each port.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        ptpIntSt   - interrupt status for
*                       ptpTrigGenInt;      Trigger generate mode Interrupt
*                       ptpEventInt;        Event Capture Interrupt 
*                       ptpUpperPortInt;    Upper Ports Interrupt   
*                       ptpIntStPortVect
*                         interrupt status for each port (bit 0 for port 0, bit 1 for port 1, etc.)
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetPTPIntStatus
(
    IN  GT_QD_DEV     *dev,
    OUT GT_PTP_INTERRUPT_STATUS        *ptpIntSt
);

/*******************************************************************************
* gptpGetPTPGlobalTime
*
* DESCRIPTION:
*       This routine gets the global timer value that is running off of the free
*        running switch core clock.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        ptpTime    - PTP global time
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetPTPGlobalTime
(
    IN  GT_QD_DEV     *dev,
    OUT GT_U32        *ptpTime
);

/*******************************************************************************
* gptpGetTimeStamped
*
* DESCRIPTION:
*        This routine retrieves the PTP port status that includes time stamp value 
*        and sequce Id that are captured by PTP logic for a PTP frame that needs 
*        to be time stamped.
*
* INPUTS:
*       port         - logical port number.
*       timeToRead    - Arr0, Arr1, or Dep time (GT_PTP_TIME enum type)
*
* OUTPUTS:
*        ptpStatus    - PTP port status
*
* RETURNS:
*       GT_OK         - on success
*       GT_FAIL     - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetTimeStamped
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN    GT_PTP_TIME    timeToRead,
    OUT GT_PTP_TS_STATUS    *ptpStatus
);

/*******************************************************************************
* gptpResetTimeStamp
*
* DESCRIPTION:
*        This routine resets PTP Time valid bit so that PTP logic can time stamp
*        a next PTP frame that needs to be time stamped.
*
* INPUTS:
*       port         - logical port number.
*       timeToReset    - Arr0, Arr1, or Dep time (GT_PTP_TIME enum type)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_FAIL     - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpResetTimeStamp
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN    GT_PTP_TIME    timeToReset
);

/*******************************************************************************
* gptpGetTSDiscardCount
*
* DESCRIPTION:
*        This routine gets Precise Time Protocol Departure frame discard counter 
*        for PTP frames that need hardware time stamping.
*
* INPUTS:
*       port         - logical port number.
*       discardCount    - discard counters
*
* OUTPUTS:
*        ptpStatus    - PTP port status
*
* RETURNS:
*       GT_OK         - on success
*       GT_FAIL     - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetTSDiscardCount
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    OUT GT_PTP_TS_DISCARD_COUNTER    *discardCount
);

/*******************************************************************************
* gptpGetOutFCSErr
*
* DESCRIPTION:
*        This routine gets Precise Time Protocol Output CRC Error counter.
*
* INPUTS:
*       port         - logical port number.
*
* OUTPUTS:
*       outFcsErr    - PTP Output CRC Error counter.
*
* RETURNS:
*       GT_OK         - on success
*       GT_FAIL     - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetOutFCSErr
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    OUT GT_U8       *outFcsErr
);

/*******************************************************************************
* gptpGetReg
*
* DESCRIPTION:
*       This routine reads PTP register.
*
* INPUTS:
*       port         - logical port number.
*       regOffset    - register to read
*
* OUTPUTS:
*        data        - register data
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetReg
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_U32        regOffset,
    OUT GT_U32        *data
);

/*******************************************************************************
* gptpSetReg
*
* DESCRIPTION:
*       This routine writes data to PTP register.
*
* INPUTS:
*       port         - logical port number
*       regOffset    - register to be written
*        data        - data to be written
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpSetReg
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_U32        regOffset,
    IN  GT_U32        data
);


/* gptpGlobalPointDataSet  */
/*******************************************************************************
* gptpGlobalPointDataSet
*
* DESCRIPTION:
*        Set indirect register pointer control data from PTP Global registers. 
*
* INPUTS:
*        point - Pointer to following index registers:
*            PTP_GLOBAL_CONFIG_PTP_MODE_REG                  
*            PTP_GLOBAL_CONFIG_PTP_EXTERNAL_TIME_FORM       
*            PTP_GLOBAL_CONFIG_1588_ING_ACT_VEC          
*            PTP_GLOBAL_CONFIG_1588_EG_ACT_VEC           
*            PTP_GLOBAL_CONFIG_AS_ING_ACT_VEC            
*            PTP_GLOBAL_CONFIG_AS_EG_ACT_VEC             
*        data -  PTP Global configuration data write to the indirect register 
*                pointed to by the point above.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpGlobalPointDataSet
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8             point,
    IN  GT_U8             data
);

/*******************************************************************************
* gptpGlobalPointDataGet
*
* DESCRIPTION:
*        Get indirect register pointer control data from PTP Global registers. 
*
* INPUTS:
*        point - Pointer to following index registers:
*            PTP_GLOBAL_CONFIG_PTP_MODE_REG                  
*            PTP_GLOBAL_CONFIG_PTP_EXTERNAL_TIME_FORM       
*            PTP_GLOBAL_CONFIG_1588_ING_ACT_VEC          
*            PTP_GLOBAL_CONFIG_1588_EG_ACT_VEC           
*            PTP_GLOBAL_CONFIG_AS_ING_ACT_VEC            
*            PTP_GLOBAL_CONFIG_AS_EG_ACT_VEC             
* OUTPUTS:
*        data -  PTP Global configuration data read from the indirect register 
*                pointed to by the point above.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpGlobalPointDataGet
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8             point,
    OUT  GT_U8            *data
);

/* PTP Global TOD Operation  */
/*******************************************************************************
* gptpTODLoadPointSet
*
* DESCRIPTION:
*        Set Time of Day Load Point Register. 
*
* INPUTS:
*        loadData - The ToDLoadPt register is used:
*          ToD Store All Registers � it is used to determine the instant in time 
*          that the selected Time Array is loaded. The load occurs at the instant
*          the PTP Global Time (TAI Global offset 0x0E & 0x0F) matches the contents
*          of this register.
*          ToD Capture � it is used to capture the instant in time that the Capture
*          occurred. On each ToD Capture, the contents of this register will be loaded
*          with the current value contained in the PTP Global Time (TAI Global offsets
*          0x0E & 0x0F).
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpTODLoadPointSet
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U32            loadData
);

/*******************************************************************************
* gptpTODLoadPointGet
*
* DESCRIPTION:
*        Set Time of Day Load Point Register. 
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        loadData - The ToDLoadPt register is used:
*          ToD Store All Registers � it is used to determine the instant in time 
*          that the selected Time Array is loaded. The load occurs at the instant
*          the PTP Global Time (TAI Global offset 0x0E & 0x0F) matches the contents
*          of this register.
*          ToD Capture � it is used to capture the instant in time that the Capture
*          occurred. On each ToD Capture, the contents of this register will be loaded
*          with the current value contained in the PTP Global Time (TAI Global offsets
*          0x0E & 0x0F).
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpTODLoadPointGet
(
    IN  GT_QD_DEV         *dev,
    OUT GT_U32            *loadData
);

/*******************************************************************************
* gptpTODOpCtrlSet
*
* DESCRIPTION:
*        Set ToD operation control. 
*
* INPUTS:
*        todOpCtrl -  following parameters:
*           ptpOpCtrlTodOp
*           ptpOpCtrlTimeArray;
*           ptpOpCtrlClkVakid;
*           ptpOpCtrlDomainNum;
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpTODOpCtrlSet
(
    IN  GT_QD_DEV             *dev,
    IN  GT_PTP_TOD_OP_CTRL    *todOpCtrl
);

/*******************************************************************************
* gptpTODOpCtrlGet
*
* DESCRIPTION:
*        Get ToD operation control. 
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        todOpCtrl -  following parameters:
*           ptpOpCtrlTodOp
*           ptpOpCtrlTimeArray;
*           ptpOpCtrlClkVakid;
*           ptpOpCtrlDomainNum;
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpTODOpCtrlGet
(
    IN  GT_QD_DEV             *dev,
    OUT  GT_PTP_TOD_OP_CTRL    *todOpCtrl
);

/*******************************************************************************
* gptpPulseSet
*
* DESCRIPTION:
*        Set PTP Pulse paraneters. 
*
* INPUTS:
*        pulsePara -  following parameters:
*             ptpPulseWidth;
*             ptp1ppsWidthRange;
*             ptp1ppsPhase;
*             ptp1ppsSelect;
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpPulseSet
(
    IN  GT_QD_DEV             *dev,
    IN  GT_PTP_PULSE_STRUCT    *pulsePara
);

/*******************************************************************************
* gptpPulseGet
*
* DESCRIPTION:
*        Get ToD operation control. 
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        pulsePara -  following parameters:
*             ptpPulseWidth;
*             ptp1ppsWidthRange;
*             ptp1ppsPhase;
*             ptp1ppsSelect;
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpPulseGet
(
    IN  GT_QD_DEV             *dev,
    OUT  GT_PTP_PULSE_STRUCT    *pulsePara
);

/*******************************************************************************
* gptpTODTimeSet
*
* DESCRIPTION:
*        Set Time Array Time of Day,. 
*
* INPUTS:
*        ts - Full TOD time structure (10 bytes).
*             The five ToD registers (at PTP Global offsets 0x13 to 0x17) 
*             contain the 10 byte representation of time used in IEEE 1588 & 
*             IEEE 802.1AS frames. These registers are used to load this 
*             representation of time into the selected Time Array on ToD Store 
*             All Registers operations. They contain the selected Time Array�s 
*             representation of this time after ToD Capture operations complete.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpTODTimeSet
(
    IN  GT_QD_DEV         *dev,
    IN  GT_PTP_TS_STRUCT    *ts
);

/*******************************************************************************
* gptpTODTimeSet
*
* DESCRIPTION:
*        Get Time Array Time of Day,. 
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        ts - Full TOD time structure (10 bytes).
*             The five ToD registers (at PTP Global offsets 0x13 to 0x17) 
*             contain the 10 byte representation of time used in IEEE 1588 & 
*             IEEE 802.1AS frames. These registers are used to load this 
*             representation of time into the selected Time Array on ToD Store 
*             All Registers operations. They contain the selected Time Array�s 
*             representation of this time after ToD Capture operations complete.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpTODTimeGet
(
    IN  GT_QD_DEV         *dev,
    OUT GT_PTP_TS_STRUCT    *ts
);

/*******************************************************************************
* gptpTODCompSet
*
* DESCRIPTION:
*        Set PTP time array time of day compensation. 
*
* INPUTS:
*        comp - PPM difference between local crystal clocking this PTP block and
*				the PTP Grand Master device that this Time Array is tracking
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpTODCompSet
(
    IN  GT_QD_DEV	*dev,
    IN  GT_U32		comp
);

/*******************************************************************************
* gptpTODCompGet
*
* DESCRIPTION:
*        Get PTP time array time of day compensation. 
*
* INPUTS:
*        None.
*
* OUTPUTS:
*		 comp - PPM difference between local crystal clocking this PTP block and
*				the PTP Grand Master device that this Time Array is tracking
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpTODCompGet
(
    IN   GT_QD_DEV	*dev,
    OUT  GT_U32		*comp
);

/*******************************************************************************
* gptp1722TimeSet
*
* DESCRIPTION:
*        Set Time Array 1722 Time of Day. 
*        The four 1722 ToD registers (at PTP Global offsets 0x18 to 0x1B) 
*        contain an 8 byte representation of time used in IEEE 1722 frames 
*        (IEEE1722 uses only the lower 32-bits of this time. The 64-bit 
*        representation is used in PCI-e and it is a simple extension of the 
*        IEEE 1722 representation of time that wraps). These registers are used
*        to load this representation of time into the selected Time Array on 
*        ToD Store All Registers operations. They contain the selected Time 
*        Array�s representation of this time after ToD Capture operations complete.*
*
* INPUTS:
*        ts_hi - High 4 bytes of Full 1722 TOD time structure (8 bytes).
*        ts_lo - Low 4 bytes of Full 1722 TOD time structure (8 bytes).
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptp1722TimeSet
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U32   ts_hi,
    IN  GT_U32   ts_lo
);

/*******************************************************************************
* gptp1722TimeGet
*
* DESCRIPTION:
*        Get Time Array 1722 Time of Day. 
*        The four 1722 ToD registers (at PTP Global offsets 0x18 to 0x1B) 
*        contain an 8 byte representation of time used in IEEE 1722 frames 
*        (IEEE1722 uses only the lower 32-bits of this time. The 64-bit 
*        representation is used in PCI-e and it is a simple extension of the 
*        IEEE 1722 representation of time that wraps). These registers are used
*        to load this representation of time into the selected Time Array on 
*        ToD Store All Registers operations. They contain the selected Time 
*        Array�s representation of this time after ToD Capture operations complete.*
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        ts_hi - High 4 bytes of Full 1722 TOD time structure (8 bytes).
*        ts_lo - Low 4 bytes of Full 1722 TOD time structure (8 bytes).
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptp1722TimeGet
(
    IN  GT_QD_DEV         *dev,
    OUT GT_U32   *ts_hi,
    OUT GT_U32   *ts_lo
);

/*******************************************************************************
* gptpModeSet
*
* DESCRIPTION:
*        Set PTP_GLOBAL_CONFIG_PTP_MODE_REG register from PTP point/data registers. 
*
* INPUTS:
*        mode -  
*          tsAtSFD    - Time Stamp At Start of Frame Delimiter
*          disPtp2Switch    - Disable PTP Signals to the Switch core
*		   accelNewPtp	 - Hardware accelerates newer PTP frames
*          altScheme    - Alternate Scheme
*          grandMstr    - Grand Master Enable.
*          oneStep    - OneStep Enable.
*          ptpMode    - PTP Mode as follows:
*                   00 = Boundary Clock
*                   01 = Peer to Peer Transparent Clock
*                   10 = End to End Transparent Clock
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpModeSet
(
    IN  GT_QD_DEV         *dev,
    IN  GT_PTP_MODE_CONFIG  *mode
);

/*******************************************************************************
* gptpModeGet
*
* DESCRIPTION:
*        Get PTP_GLOBAL_CONFIG_PTP_MODE_REG register from PTP point/data registers. 
*
* INPUTS:
*        None   .
*
* OUTPUTS:
*          tsAtSFD    - Time Stamp At Start of Frame Delimiter
*          disPtp2Switch    - Disable PTP Signals to the Switch core
*		   accelNewPtp	 - Hardware accelerates newer PTP frames
*          altScheme    - Alternate Scheme
*          grandMstr    - Grand Master Enable.
*          oneStep    - OneStep Enable.
*          ptpMode    - PTP Mode as follows:
*                   00 = Boundary Clock
*                   01 = Peer to Peer Transparent Clock
*                   10 = End to End Transparent Clock
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpModeGet
(
    IN  GT_QD_DEV           *dev,
    OUT GT_PTP_MODE_CONFIG  *mode
);


/*******************************************************************************
* gptpSetExtTimeFormat
*
* DESCRIPTION:
*        Set PTP_GLOBAL_CONFIG_PTP_EXTERNAL_TIME_FORM from PTP point/data registers. 
*
* INPUTS:
*        format-
*			intPTPTime    - Internal PTP Time.
*			extMask    - External Mask on the lower bits of the selected 1722 time.
*			ext1722Time    - IEEE 1722 Time Format to External switch blocks.
*			extTASel    - Time Array Selection for External switch blocks.
*
* OUTPUTS:
*          None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpSetExtTimeFormat
(
	IN  GT_QD_DEV         *dev,
    IN  GT_PTP_EXTERNAL_TIME_FORM  *format
);

/*******************************************************************************
* gptpGetExtTimeFormat
*
* DESCRIPTION:
*        Get PTP_GLOBAL_CONFIG_PTP_EXTERNAL_TIME_FORM from PTP point/data registers. 
*
* INPUTS:
*		 None.
*        
* OUTPUTS:
*        format-
*			intPTPTime    - Internal PTP Time.
*			extMask    - External Mask on the lower bits of the selected 1722 time.
*			ext1722Time    - IEEE 1722 Time Format to External switch blocks.
*			extTASel    - Time Array Selection for External switch blocks.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpGetExtTimeFormat
(
    IN  GT_QD_DEV           *dev,
    OUT GT_PTP_EXTERNAL_TIME_FORM  *format
);

/*******************************************************************************
* gptpSetMeanPathDelay
*
* DESCRIPTION:
*		This routine sets ingress mean path delay on a port.
* 
* INPUTS:
*		delay	- cable delay between this port and its link partner in unsiged
*				  nano seconds
*
* OUTPUTS:
*		None.
*
* RETURNS:
*		GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*		None
*
*******************************************************************************/
GT_STATUS gptpSetMeanPathDelay
(
	IN   GT_QD_DEV	*dev,
    IN   GT_LPORT	port,
    IN   GT_U16		delay
);

/*******************************************************************************
* gptpGetMeanPathDelay
*
* DESCRIPTION:
*		This routine gets ingress mean path delay on a port.
* 
* INPUTS:
*		None.
*
* OUTPUTS:
*		delay	- cable delay between this port and its link partner in unsiged
*				  nano seconds
*
* RETURNS:
*		GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*		None
*
*******************************************************************************/

GT_STATUS gptpGetMeanPathDelay
(
	IN   GT_QD_DEV	*dev,
    IN   GT_LPORT	port,
    OUT  GT_U16		*delay
);

/*******************************************************************************
* gptpSetIngPathDelayAsym
*
* DESCRIPTION:
*		This routine sets ingress path delay asymmetry on a port.
* 
* INPUTS:
*		asym	- bit 15 is path delay asymmetry sign
*						0 - the path delay asymmetry number is added
*						1 - the path delay asymmetry number is subtracted
*
*				  bit 14:0 indicates the assymmetry value beyond the mean path delay
*				  that needs to be added or subtracted for more accurate cable
*				  measurements. 
*
* OUTPUTS:
*		None.
*
* RETURNS:
*		GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*		None
*
*******************************************************************************/
GT_STATUS gptpSetIngPathDelayAsym
(
	IN   GT_QD_DEV	*dev,
    IN   GT_LPORT	port,
    IN   GT_U16		asym
);

/*******************************************************************************
* gptpGetIngPathDelayAsym
*
* DESCRIPTION:
*		This routine gets ingress path delay asymmetry on a port.
* 
* INPUTS:
*		None.
*
* OUTPUTS:
*		asym	- bit 15 is path delay asymmetry sign
*						0 - the path delay asymmetry number is added
*						1 - the path delay asymmetry number is subtracted
*
*				  bit 14:0 indicates the assymmetry value beyond the mean path delay
*				  that needs to be added or subtracted for more accurate cable
*				  measurements. 
*
* RETURNS:
*		GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*		None
*
*******************************************************************************/
GT_STATUS gptpGetIngPathDelayAsym
(
	IN   GT_QD_DEV	*dev,
    IN   GT_LPORT	port,
    OUT  GT_U16		*asym
);

/*******************************************************************************
* gptpSetEgPathDelayAsym
*
* DESCRIPTION:
*		This routine sets egress path delay asymmetry on a port.
* 
* INPUTS:
*		asym	- bit 15 is path delay asymmetry sign
*						0 - the path delay asymmetry number is added
*						1 - the path delay asymmetry number is subtracted
*
*				  bit 14:0 indicates the assymmetry value beyond the mean path delay
*				  that needs to be added or subtracted for more accurate cable
*				  measurements. 
*
* OUTPUTS:
*		None.
*
* RETURNS:
*		GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*		None
*
*******************************************************************************/
GT_STATUS gptpSetEgPathDelayAsym
(
	IN   GT_QD_DEV	*dev,
    IN   GT_LPORT	port,
    IN   GT_U16		asym
);

/*******************************************************************************
* gptpGetEgPathDelayAsym
*
* DESCRIPTION:
*		This routine gets egress path delay asymmetry on a port.
* 
* INPUTS:
*		None.
*
* OUTPUTS:
*		asym	- bit 15 is path delay asymmetry sign
*						0 - the path delay asymmetry number is added
*						1 - the path delay asymmetry number is subtracted
*
*				  bit 14:0 indicates the assymmetry value beyond the mean path delay
*				  that needs to be added or subtracted for more accurate cable
*				  measurements. 
*
* RETURNS:
*		GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*		None
*
*******************************************************************************/
GT_STATUS gptpGetEgPathDelayAsym
(
	IN   GT_QD_DEV	*dev,
    IN   GT_LPORT	port,
    OUT  GT_U16		*asym
);

/* gptpGetIngActVec  */
/*******************************************************************************
* gptpGetIngActVec
*
* DESCRIPTION:
*        Get IEEE 1588 or IEEE 802.1AS Ingress Action Vectors. 
*
* INPUTS:
*		 spec - PTP_IEEE_1588 or PTP_IEEE_802_1AS.
*        
* OUTPUTS:
*        vec - IEEE 1588 or IEEE 802.1AS Ingress Action Vector 2 to 0.
*			
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpGetIngActVec
(
    IN  GT_QD_DEV	*dev,
	IN  GT_PTP_SPEC spec,
    OUT GT_U32		*vec
);

/*******************************************************************************
* gptpSetIngActVec
*
* DESCRIPTION:
*        Get IEEE 1588 or IEEE 802.1AS Ingress Action Vectors. 
*
* INPUTS:
*		 spec - PTP_IEEE_1588 or PTP_IEEE_802_1AS.
*		 vec - IEEE 1588 or IEEE 802.1AS Ingress Action Vector 2 to 0.
*        
* OUTPUTS:
*        None.
*			
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpSetIngActVec
(
    IN  GT_QD_DEV			*dev,
	IN  GT_PTP_SPEC			spec,
    IN  GT_PTP_EG_ACT_VEC	vec
);

/*******************************************************************************
* gptpGetEgActVec
*
* DESCRIPTION:
*        Get IEEE_1588 or IEEE 802.1AS Egress Action Vectors. 
*
* INPUTS:
*		 spec - PTP_IEEE_1588 or PTP_IEEE_802_1AS.
*        
* OUTPUTS:
*        vec - IEEE_1588 or IEEE 802.1AS Egress Action Vector 3 to 0.
*			
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpGetEgActVec
(
    IN  GT_QD_DEV			*dev,
	IN  GT_PTP_SPEC			spec,
    OUT GT_PTP_EG_ACT_VEC	*vec
);

/*******************************************************************************
* gptpSetEgActVec
*
* DESCRIPTION:
*        Set IEEE_1588 or IEEE 802.1AS Egress Action Vectors. 
*
* INPUTS:
*		 spec - PTP_IEEE_1588 or PTP_IEEE_802_1AS.
*        vec - IEEE_1588 or IEEE 802.1AS Egress Action Vector 3 to 0.
*        
* OUTPUTS:
*		 None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpSetEgActVec
(
    IN  GT_QD_DEV	*dev,
	IN  GT_PTP_SPEC spec,
    IN  GT_PTP_EG_ACT_VEC	vec
);

/* Amber APIs */

/* gtBrgFdb.c */

/*******************************************************************************
* gfdbGetMacAvb
*
* DESCRIPTION:
*        ATU MAC entry in AVB mode.
*        When enabled, ATU entries operate in AVB mode:
*
*        GT_ATU_UC_STATE - support
*            GT_UC_NO_PRI_STATIC_AVB_ENTRY, and 
*            GT_UC_STATIC_AVB_ENTRY
*
*        GT_ATU_MC_STATE - support
*            GT_MC_STATIC_AVB_ENTRY, and
*            GT_MC_PRIO_STATIC_AVB_ENTRY
*
*        When disabled, ATU entries operate in non-AVB mode:
*
*        GT_ATU_UC_STATE - support
*            GT_UC_NO_PRI_STATIC_NRL, and 
*            GT_UC_STATIC_NRL
*
*        GT_ATU_MC_STATE - support
*            GT_MC_STATIC_UNLIMITED_RATE, and
*            GT_MC_PRIO_STATIC_UNLIMITED_RATE
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        mode  - GT_TRUE if MacAvb is enabled, GT_FALSE otherwise
*
* RETURNS:
*        GT_OK           - on success
*        GT_FAIL         - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*
*******************************************************************************/
GT_STATUS gfdbGetMacAvb
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL     *mode
);

/*******************************************************************************
* gfdbSetMacAvb
*
* DESCRIPTION:
*        ATU MAC entry in AVB mode.
*        When enabled, ATU entries operate in AVB mode:
*
*        GT_ATU_UC_STATE - support
*            GT_UC_NO_PRI_STATIC_AVB_ENTRY, and 
*            GT_UC_STATIC_AVB_ENTRY
*
*        GT_ATU_MC_STATE - support
*            GT_MC_STATIC_AVB_ENTRY, and
*            GT_MC_PRIO_STATIC_AVB_ENTRY
*
*        When disabled, ATU entries operate in non-AVB mode:
*
*        GT_ATU_UC_STATE - support
*            GT_UC_NO_PRI_STATIC_NRL, and 
*            GT_UC_STATIC_NRL
*
*        GT_ATU_MC_STATE - support
*            GT_MC_STATIC_UNLIMITED_RATE, and
*            GT_MC_PRIO_STATIC_UNLIMITED_RATE
*
* INPUTS:
*        mode - GT_TRUE to enable MacAvb, GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gfdbSetMacAvb
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL        mode
);

/*******************************************************************************
* gfdbGetPortAtuLearnLimit
*
* DESCRIPTION:
*      Port's auto learning limit. When the limit is non-zero value, the number
*        of MAC addresses that can be learned on this port are limited to the value
*        specified in this API. When the learn limit has been reached any frame 
*        that ingresses this port with a source MAC address not already in the 
*        address database that is associated with this port will be discarded. 
*        Normal auto-learning will resume on the port as soon as the number of 
*        active unicast MAC addresses associated to this port is less than the 
*        learn limit.
*        CPU directed ATU Load, Purge, or Move will not have any effect on the 
*        learn limit.
*        This feature is disabled when the limit is zero.
*        The following care is needed when enabling this feature:
*            1) dsable learning on the ports
*            2) flush all non-static addresses in the ATU
*            3) define the desired limit for the ports
*            4) re-enable learing on the ports
*
* INPUTS:
*        port  - logical port number
*                                              
* OUTPUTS:
*        limit - auto learning limit ( 0 ~ 255 )
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if limit > 0xFF
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gfdbGetPortAtuLearnLimit
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT      port,
    OUT GT_U32       *limit
);

/* gtPCSCtrl.c */

/*******************************************************************************
* gpcsSetFCMode
*
* DESCRIPTION:
*        This routine sets full duplex flow control mode
*
* INPUTS:
*        port - the logical port number.
*        mode - flow control mode
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsSetFCMode
(
	IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_PORT_FC_MODE      mode
);

/*******************************************************************************
* gpcsGetFCMode
*
* DESCRIPTION:
*        This routine gets full duplex flow control mode
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        mode - flow control mode
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetFCMode
(
	IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    OUT GT_PORT_FC_MODE      *mode
);

/*******************************************************************************
* gprtSetFCPath
*
* DESCRIPTION:
*        This routine sets flow control QC path considerations bit.
*
*        If set, the QC will use its FC algorithm only when both the source port
*		 of a frame and its destination port have FC enabled.
*		 If cleared, the QC will use its FC algorithm whenever the source port
*		 of a frame has FC enabled.
*
* INPUTS:
*        port - the logical port number.
*		 state - GT_TRUE if connected or GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        
*
*******************************************************************************/
GT_STATUS gprtSetFCPath
(
	IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_BOOL      state
);

/*******************************************************************************
* gprtGetFCPath
*
* DESCRIPTION:
*        This routine retrives the information regarding flow control QC path
*		 considerations.
*
*        If set, the QC will use its FC algorithm only when both the source port
*		 of a frame and its destination port have FC enabled.
*		 If cleared, the QC will use its FC algorithm whenever the source port
*		 of a frame has FC enabled.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE if connected or GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        
*
*******************************************************************************/
GT_STATUS gprtGetFCPath
(
	IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL     *state
);

/*******************************************************************************
* gpcsGetRGMIITimingDelay
*
* DESCRIPTION:
*        RGMII receive/transmit Timing Control. This api adds delay to RXCLK for
*        IND inputs and GTXCLK for OUTD outputs when port is in RGMII mode.
*        Change to this bit are disruptive to normal operation. Hence any changes
*        to this register must be done only while the port's link is down.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        rxmode - GT_FALSE for default setup, GT_TRUE for adding delay to rxclk
*        txmode - GT_FALSE for default setup, GT_TRUE for adding delay to txclk
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gpcsGetRGMIITimingDelay
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *rxmode,
    OUT GT_BOOL      *txmode
);

/*******************************************************************************
* gpcsSetRGMIITimingDelay
*
* DESCRIPTION:
*        RGMII receive/transmit Timing Control. This api adds delay to RXCLK for
*        IND inputs and GTXCLK for OUTD outputs when port is in RGMII mode.
*        Change to this bit are disruptive to normal operation. Hence any changes
*        to this register must be done only while the port's link is down.
*
* INPUTS:
*        port - the logical port number.
*        rxmode - GT_FALSE for default setup, GT_TRUE for adding delay to rxclk
*        txmode - GT_FALSE for default setup, GT_TRUE for adding delay to txclk
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gpcsSetRGMIITimingDelay
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_BOOL      rxmode,
    IN  GT_BOOL      txmode
);

/*******************************************************************************
* gpcsSetRGMIIAutoNego
*
* DESCRIPTION:
*        SetRGMII Auto-Negotiation enable. When this bit is set to a one, RGMII in-band
*        auto-negotiation is supported where the results of the auto-negotiation
*        are sent to the port�s MAC if the RGMII Result Enable bit (Port offset
*        0x17) is set to a one.
*
* INPUTS:
*        port - the logical port number.
*        en - GT_FALSE to disable RGMII Auto-Negotiation, 
*             GT_TRUE to enable RGMII Auto-Negotiation
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gpcsSetRGMIIAutoNego
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_BOOL      en
);

/*******************************************************************************
* gpcsGetRGMIIAutoNego
*
* DESCRIPTION:
*        Get RGMII Auto-Negotiation enable. When this bit is set to a one, RGMII in-band
*        auto-negotiation is supported where the results of the auto-negotiation
*        are sent to the port�s MAC if the RGMII Result Enable bit (Port offset
*        0x17) is set to a one.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        en - GT_FALSE to disable RGMII Auto-Negotiation, 
*             GT_TRUE to enable RGMII Auto-Negotiation
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gpcsGetRGMIIAutoNego
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *en
);

/*******************************************************************************
* gpcsSetForceSpeedEn
*
* DESCRIPTION:
*        Set Force Speed. When this bit is set to a one the speed for this 
*        port�s MAC will be forced to the value in the SpdValue and the AltSpeed 
*        registers regardless of what the normal speed value would be. 
*        When this bit is cleared to a zero, normal speed detection occurs.
*
* INPUTS:
*        port - the logical port number.
*        en - GT_FALSE to Reset Force Speed, 
*             GT_TRUE to Set Force Speed
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gpcsSetForceSpeedEn
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_BOOL      en
);

/*******************************************************************************
* gpcsGetForceSpeedEn
*
* DESCRIPTION:
*        Get Force Speed. When this bit is set to a one the speed for this 
*        port�s MAC will be forced to the value in the SpdValue and the AltSpeed 
*        registers regardless of what the normal speed value would be. 
*        When this bit is cleared to a zero, normal speed detection occurs.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        en - GT_FALSE to Reset Force Speed, 
*             GT_TRUE to Set Force Speed
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gpcsGetForceSpeedEn
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *en
);

/*******************************************************************************
* gpcsSetAltSpeed
*
* DESCRIPTION:
*        Set Alternate Speed Mode. When Port 2, 5 or 6�s C_Mode is 0x0 or 0x1 
*        (Port offset 0x00) this bit can be used along with the SpdValue bits 
*        below to force the port�s speed as follows: 0 = Normal 10, 100, 1000 or 
*        10G speed 1 = Alternate speed as defined in the SpdValue register
*
* INPUTS:
*        port - the logical port number.
*        en - GT_FALSE to Reset Alternate Speed Mode, 
*             GT_TRUE to Set Alternate Speed Mode
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gpcsSetAltSpeed
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_BOOL      en
);

/*******************************************************************************
* gpcsGetAltSpeed
*
* DESCRIPTION:
*        Get Alternate Speed Mode. When Port 2, 5 or 6�s C_Mode is 0x0 or 0x1 
*        (Port offset 0x00) this bit can be used along with the SpdValue bits 
*        below to force the port�s speed as follows: 0 = Normal 10, 100, 1000 or 
*        10G speed 1 = Alternate speed as defined in the SpdValue register
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        en - GT_FALSE to Reset Alternate Speed Mode, 
*             GT_TRUE to Set Alternate Speed Mode
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gpcsGetAltSpeed
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *en
);

/* gtPortLed.c */

/*******************************************************************************
* gprtSetLED
*
* DESCRIPTION:
*        This API allows to configure 4 LED sections, Pulse stretch, Blink rate,
*        and special controls.
*
* INPUTS:
*        port    - the logical port number
*        cfg     - GT_LED_CFG value
*        value     - value to be configured
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gprtSetLED
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_LED_CFG    cfg,
    IN  GT_U32        value
);


/*******************************************************************************
* gprtGetLED
*
* DESCRIPTION:
*        This API allows to retrieve 4 LED sections, Pulse stretch, Blink rate,
*        and special controls.
*
* INPUTS:
*        port    - the logical port number
*        cfg     - GT_LED_CFG value
*
* OUTPUTS:
*        value     - configured value
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gprtGetLED
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_LED_CFG    cfg,
    OUT GT_U32        *value
);





/* gtPortStatus.c */

/*******************************************************************************
* gprtGetQSizePerQPri
*
* DESCRIPTION:
*        This routine gets egress queue size for port's each QPri (0 ~ 3).
*
* INPUTS:
*        port - the logical port number
*
* OUTPUTS:
*        counts - egress queue size per QPri (should be 4 * 16bytes)
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtGetQSizePerQPri
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_U16        *counts
);


/* gtSysCtrl.c */


/*******************************************************************************
* gsysGetARPwoBC
*
* DESCRIPTION:
*       ARP detection without Broadcast checking. When enabled the switch core
*       does not check for a Btoadcast MAC address as part of the ARP frame
*       detection. It only checkes the Ether Type (0x0806).
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       en - GT_TRUE if enabled, GT_FALSE otherwise.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on bad parameter
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gsysGetARPwoBC
(
    IN  GT_QD_DEV     *dev,
    IN  GT_BOOL        *en
);

/*******************************************************************************
* gsysSetARPwoBC
*
* DESCRIPTION:
*       ARP detection without Broadcast checking. When enabled the switch core
*       does not check for a Btoadcast MAC address as part of the ARP frame
*       detection. It only checkes the Ether Type (0x0806).
*
* INPUTS:
*       en - GT_TRUE to enable, GT_FALSE otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gsysSetARPwoBC
(
    IN  GT_QD_DEV    *dev,
    IN  GT_BOOL     en
);


/*******************************************************************************
* gsysGetCLK125En
*
* DESCRIPTION:
*        Clock 125MHz Enable.
*        When this feature is enabled, the CLK125 pin has a free running 125 MHz
*        clock output.
*        When it's disabled, the CLK125 pin will be in tri-state.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        en - GT_TRUE if 125MHz clock is enabled, GT_FALSE otherwise.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysGetCLK125En
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL      *en
);


/*******************************************************************************
* gsysSetCLK125En
*
* DESCRIPTION:
*        Clock 125MHz Enable.
*        When this feature is enabled, the CLK125 pin has a free running 125 MHz
*        clock output.
*        When it's disabled, the CLK125 pin will be in tri-state.
*
* INPUTS:
*        en - GT_TRUE to enable 125 MHz clock, GT_FALSE otherwise.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysSetCLK125En
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL        en
);


/* gtPriTable.c */

/*******************************************************************************
* gsysSetFPriOverrideTable
*
* DESCRIPTION:
*         Frame Priority Override.
*        When a frame enters a port, its type is determined and the type is used 
*        to access the Frame Priority Table. If the type's fPriEn (in GT_FPRI_TBL_ENTRY
*        structure) is enabled, then the frame's Frame Priority will be overridden
*        with the value written in fPriority (in GT_FPRI_TBL_ENTRY structure).
*        Frame Types supported are:
*            FTYPE_DSA_TO_CPU_BPDU -
*                Used on multicast DSA To_CPU frames with a Code of 0x0 (BPDU/MGMT).
*                Not used on non-DSA Control frames.
*            FTYPE_DSA_TO_CPU_F2R -
*                Used on DSA To_CPU frames with a Code of 0x1 (Frame to Register
*                Reply). Not used on non-DSA Control frames.
*            FTYPE_DSA_TO_CPU_IGMP -
*                Used on DSA To_CPU frames with a Code of 0x2 (IGMP/MLD Trap)
*                and on non-DSA Control frames that are IGMP or MLD trapped
*            FTYPE_DSA_TO_CPU_TRAP -
*                Used on DSA To_CPU frames with a Code of 0x3 (Policy Trap) and
*                on non-DSA Control frames that are Policy Trapped
*            FTYPE_DSA_TO_CPU_ARP -
*                Used on DSA To_CPU frames with a Code of 0x4 (ARP Mirror) and
*                on non-DSA Control frames that are ARP Mirrored (see gprtSetARPtoCPU API).
*            FTYPE_DSA_TO_CPU_MIRROR -
*                Used on DSA To_CPU frames with a Code of 0x5 (Policy Mirror) and
*                on non-DSA Control frames that are Policy Mirrored (see gprtSetPolicy API).
*            FTYPE_DSA_TO_CPU_RESERVED -
*                Used on DSA To_CPU frames with a Code of 0x6 (Reserved). Not
*                used on non-DSA Control frames.
*            FTYPE_DSA_TO_CPU_UCAST_MGMT -
*                Used on unicast DSA To_CPU frames with a Code of 0x0 (unicast
*                MGMT). Not used on non-DSA Control frames.
*            FTYPE_DSA_FROM_CPU -
*                Used on DSA From_CPU frames. Not used on non-DSA Control frame
*            FTYPE_DSA_CROSS_CHIP_FC -
*                Used on DSA Cross Chip Flow Control frames (To_Sniffer Flow
*                Control). Not used on non-DSA Control frames.
*            FTYPE_DSA_CROSS_CHIP_EGRESS_MON -
*                Used on DSA Cross Chip Egress Monitor frames (To_Sniffer Tx).
*                Not used on non-DSA Control frames.
*            FTYPE_DSA_CROSS_CHIP_INGRESS_MON -
*                Used on DSA Cross Chip Ingress Monitor frames (To_Sniffer Rx).
*                Not used on non-DSA Control frames.
*            FTYPE_PORT_ETYPE_MATCH -
*                Used on normal network ports (see gprtSetFrameMode API)
*                on frames whose Ethertype matches the port's PortEType register.
*                Not used on non-DSA Control frames.
*            FTYPE_BCAST_NON_DSA_CONTROL -
*                Used on Non-DSA Control frames that contain a Broadcast
*                destination address. Not used on DSA Control frames.
*            FTYPE_PPPoE_NON_DSA_CONTROL -
*                Used on Non-DSA Control frames that contain an Ether Type 0x8863
*                (i.e., PPPoE frames). Not used on DSA Control frames.
*            FTYPE_IP_NON_DSA_CONTROL -
*                Used on Non-DSA Control frames that contain an IPv4 or IPv6 Ether
*                Type. Not used on DSA Control frames.
*
* INPUTS:
*       fType - frame type (GT_PRI_OVERRIDE_FTYPE)
*       entry - Frame Priority Override Table entry (GT_FPRI_TBL_ENTRY)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM     - on unknown frame type
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gsysSetFPriOverrideTable
(
    IN  GT_QD_DEV     *dev,
    IN  GT_PRI_OVERRIDE_FTYPE    fType,
    IN  GT_FPRI_TBL_ENTRY    *entry
);


/*******************************************************************************
* gsysGetFPriOverrideTable
*
* DESCRIPTION:
*         Frame Priority Override.
*        When a frame enters a port, its type is determined and the type is used 
*        to access the Frame Priority Table. If the type's fPriEn (in GT_FPRI_TBL_ENTRY
*        structure) is enabled, then the frame's Frame Priority will be overridden
*        with the value written in fPriority (in GT_FPRI_TBL_ENTRY structure).
*        Frame Types supported are:
*            FTYPE_DSA_TO_CPU_BPDU -
*                Used on multicast DSA To_CPU frames with a Code of 0x0 (BPDU/MGMT).
*                Not used on non-DSA Control frames.
*            FTYPE_DSA_TO_CPU_F2R -
*                Used on DSA To_CPU frames with a Code of 0x1 (Frame to Register
*                Reply). Not used on non-DSA Control frames.
*            FTYPE_DSA_TO_CPU_IGMP -
*                Used on DSA To_CPU frames with a Code of 0x2 (IGMP/MLD Trap)
*                and on non-DSA Control frames that are IGMP or MLD trapped
*            FTYPE_DSA_TO_CPU_TRAP -
*                Used on DSA To_CPU frames with a Code of 0x3 (Policy Trap) and
*                on non-DSA Control frames that are Policy Trapped
*            FTYPE_DSA_TO_CPU_ARP -
*                Used on DSA To_CPU frames with a Code of 0x4 (ARP Mirror) and
*                on non-DSA Control frames that are ARP Mirrored (see gprtSetARPtoCPU API).
*            FTYPE_DSA_TO_CPU_MIRROR -
*                Used on DSA To_CPU frames with a Code of 0x5 (Policy Mirror) and
*                on non-DSA Control frames that are Policy Mirrored (see gprtSetPolicy API).
*            FTYPE_DSA_TO_CPU_RESERVED -
*                Used on DSA To_CPU frames with a Code of 0x6 (Reserved). Not
*                used on non-DSA Control frames.
*            FTYPE_DSA_TO_CPU_UCAST_MGMT -
*                Used on unicast DSA To_CPU frames with a Code of 0x0 (unicast
*                MGMT). Not used on non-DSA Control frames.
*            FTYPE_DSA_FROM_CPU -
*                Used on DSA From_CPU frames. Not used on non-DSA Control frame
*            FTYPE_DSA_CROSS_CHIP_FC -
*                Used on DSA Cross Chip Flow Control frames (To_Sniffer Flow
*                Control). Not used on non-DSA Control frames.
*            FTYPE_DSA_CROSS_CHIP_EGRESS_MON -
*                Used on DSA Cross Chip Egress Monitor frames (To_Sniffer Tx).
*                Not used on non-DSA Control frames.
*            FTYPE_DSA_CROSS_CHIP_INGRESS_MON -
*                Used on DSA Cross Chip Ingress Monitor frames (To_Sniffer Rx).
*                Not used on non-DSA Control frames.
*            FTYPE_PORT_ETYPE_MATCH -
*                Used on normal network ports (see gprtSetFrameMode API)
*                on frames whose Ethertype matches the port's PortEType register.
*                Not used on non-DSA Control frames.
*            FTYPE_BCAST_NON_DSA_CONTROL -
*                Used on Non-DSA Control frames that contain a Broadcast
*                destination address. Not used on DSA Control frames.
*            FTYPE_PPPoE_NON_DSA_CONTROL -
*                Used on Non-DSA Control frames that contain an Ether Type 0x8863
*                (i.e., PPPoE frames). Not used on DSA Control frames.
*            FTYPE_IP_NON_DSA_CONTROL -
*                Used on Non-DSA Control frames that contain an IPv4 or IPv6 Ether
*                Type. Not used on DSA Control frames.
*
* INPUTS:
*       fType - frame type (GT_PRI_OVERRIDE_FTYPE)
*
* OUTPUTS:
*       entry - Frame Priority Override Table entry (GT_FPRI_TBL_ENTRY)
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_BAD_PARAM     - on unknown frame type
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gsysGetFPriOverrideTable
(
    IN  GT_QD_DEV     *dev,
    IN  GT_PRI_OVERRIDE_FTYPE    fType,
    OUT GT_FPRI_TBL_ENTRY    *entry
);


/*******************************************************************************
* gsysSetQPriAvbOverrideTable
*
* DESCRIPTION:
*         Queue Priority Override for AVB enabled ports or AvbOverride enabled ports.
*        When a frame enters a AVB port, its type is determined and the type is used 
*        to access the Queue Priority Table. If the type's qPriEn (in GT_QPRI_TBL_ENTRY
*        structure) is enabled, then the frame's Queue Priority will be overridden
*        with the value written in qPriority (in GT_QPRI_TBL_ENTRY structure).
*        Frame Types supported are:
*            FTYPE_DSA_TO_CPU_BPDU -
*                Used on multicast DSA To_CPU frames with a Code of 0x0 (BPDU/MGMT).
*                Not used on non-DSA Control frames.
*            FTYPE_DSA_TO_CPU_F2R -
*                Used on DSA To_CPU frames with a Code of 0x1 (Frame to Register
*                Reply). Not used on non-DSA Control frames.
*            FTYPE_DSA_TO_CPU_IGMP -
*                Used on DSA To_CPU frames with a Code of 0x2 (IGMP/MLD Trap)
*                and on non-DSA Control frames that are IGMP or MLD trapped
*            FTYPE_DSA_TO_CPU_TRAP -
*                Used on DSA To_CPU frames with a Code of 0x3 (Policy Trap) and
*                on non-DSA Control frames that are Policy Trapped
*            FTYPE_DSA_TO_CPU_ARP -
*                Used on DSA To_CPU frames with a Code of 0x4 (ARP Mirror) and
*                on non-DSA Control frames that are ARP Mirrored (see gprtSetARPtoCPU API).
*            FTYPE_DSA_TO_CPU_MIRROR -
*                Used on DSA To_CPU frames with a Code of 0x5 (Policy Mirror) and
*                on non-DSA Control frames that are Policy Mirrored (see gprtSetPolicy API).
*            FTYPE_DSA_TO_CPU_RESERVED -
*                Used on DSA To_CPU frames with a Code of 0x6 (Reserved). Not
*                used on non-DSA Control frames.
*            FTYPE_DSA_TO_CPU_UCAST_MGMT -
*                Used on unicast DSA To_CPU frames with a Code of 0x0 (unicast
*                MGMT). Not used on non-DSA Control frames.
*            FTYPE_DSA_FROM_CPU -
*                Used on DSA From_CPU frames. Not used on non-DSA Control frame
*            FTYPE_DSA_CROSS_CHIP_FC -
*                Used on DSA Cross Chip Flow Control frames (To_Sniffer Flow
*                Control). Not used on non-DSA Control frames.
*            FTYPE_DSA_CROSS_CHIP_EGRESS_MON -
*                Used on DSA Cross Chip Egress Monitor frames (To_Sniffer Tx).
*                Not used on non-DSA Control frames.
*            FTYPE_DSA_CROSS_CHIP_INGRESS_MON -
*                Used on DSA Cross Chip Ingress Monitor frames (To_Sniffer Rx).
*                Not used on non-DSA Control frames.
*            FTYPE_PORT_ETYPE_MATCH -
*                Used on normal network ports (see gprtSetFrameMode API)
*                on frames whose Ethertype matches the port's PortEType register.
*                Not used on non-DSA Control frames.
*            FTYPE_BCAST_NON_DSA_CONTROL -
*                Used on Non-DSA Control frames that contain a Broadcast
*                destination address. Not used on DSA Control frames.
*            FTYPE_PPPoE_NON_DSA_CONTROL -
*                Used on Non-DSA Control frames that contain an Ether Type 0x8863
*                (i.e., PPPoE frames). Not used on DSA Control frames.
*            FTYPE_IP_NON_DSA_CONTROL -
*                Used on Non-DSA Control frames that contain an IPv4 or IPv6 Ether
*                Type. Not used on DSA Control frames.
*
* INPUTS:
*       fType - frame type (GT_PRI_OVERRIDE_FTYPE)
*       entry - Q Priority Override Table entry (GT_QPRI_TBL_ENTRY)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM     - on unknown frame type
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gsysSetQPriAvbOverrideTable
(
    IN  GT_QD_DEV     *dev,
    IN  GT_PRI_OVERRIDE_FTYPE    fType,
    IN  GT_QPRI_TBL_ENTRY    *entry
);


/*******************************************************************************
* gsysGetQPriAvbOverrideTable
*
* DESCRIPTION:
*         Queue Priority Override for AVB enabled ports or AvbOverride enabled ports.
*        When a frame enters a AVB port, its type is determined and the type is used 
*        to access the Queue Priority Table. If the type's qPriEn (in GT_QPRI_TBL_ENTRY
*        structure) is enabled, then the frame's Queue Priority will be overridden
*        with the value written in qPriority (in GT_QPRI_TBL_ENTRY structure).
*        Frame Types supported are:
*            FTYPE_DSA_TO_CPU_BPDU -
*                Used on multicast DSA To_CPU frames with a Code of 0x0 (BPDU/MGMT).
*                Not used on non-DSA Control frames.
*            FTYPE_DSA_TO_CPU_F2R -
*                Used on DSA To_CPU frames with a Code of 0x1 (Frame to Register
*                Reply). Not used on non-DSA Control frames.
*            FTYPE_DSA_TO_CPU_IGMP -
*                Used on DSA To_CPU frames with a Code of 0x2 (IGMP/MLD Trap)
*                and on non-DSA Control frames that are IGMP or MLD trapped
*            FTYPE_DSA_TO_CPU_TRAP -
*                Used on DSA To_CPU frames with a Code of 0x3 (Policy Trap) and
*                on non-DSA Control frames that are Policy Trapped
*            FTYPE_DSA_TO_CPU_ARP -
*                Used on DSA To_CPU frames with a Code of 0x4 (ARP Mirror) and
*                on non-DSA Control frames that are ARP Mirrored (see gprtSetARPtoCPU API).
*            FTYPE_DSA_TO_CPU_MIRROR -
*                Used on DSA To_CPU frames with a Code of 0x5 (Policy Mirror) and
*                on non-DSA Control frames that are Policy Mirrored (see gprtSetPolicy API).
*            FTYPE_DSA_TO_CPU_RESERVED -
*                Used on DSA To_CPU frames with a Code of 0x6 (Reserved). Not
*                used on non-DSA Control frames.
*            FTYPE_DSA_TO_CPU_UCAST_MGMT -
*                Used on unicast DSA To_CPU frames with a Code of 0x0 (unicast
*                MGMT). Not used on non-DSA Control frames.
*            FTYPE_DSA_FROM_CPU -
*                Used on DSA From_CPU frames. Not used on non-DSA Control frame
*            FTYPE_DSA_CROSS_CHIP_FC -
*                Used on DSA Cross Chip Flow Control frames (To_Sniffer Flow
*                Control). Not used on non-DSA Control frames.
*            FTYPE_DSA_CROSS_CHIP_EGRESS_MON -
*                Used on DSA Cross Chip Egress Monitor frames (To_Sniffer Tx).
*                Not used on non-DSA Control frames.
*            FTYPE_DSA_CROSS_CHIP_INGRESS_MON -
*                Used on DSA Cross Chip Ingress Monitor frames (To_Sniffer Rx).
*                Not used on non-DSA Control frames.
*            FTYPE_PORT_ETYPE_MATCH -
*                Used on normal network ports (see gprtSetFrameMode API)
*                on frames whose Ethertype matches the port's PortEType register.
*                Not used on non-DSA Control frames.
*            FTYPE_BCAST_NON_DSA_CONTROL -
*                Used on Non-DSA Control frames that contain a Broadcast
*                destination address. Not used on DSA Control frames.
*            FTYPE_PPPoE_NON_DSA_CONTROL -
*                Used on Non-DSA Control frames that contain an Ether Type 0x8863
*                (i.e., PPPoE frames). Not used on DSA Control frames.
*            FTYPE_IP_NON_DSA_CONTROL -
*                Used on Non-DSA Control frames that contain an IPv4 or IPv6 Ether
*                Type. Not used on DSA Control frames.
*
* INPUTS:
*       fType - frame type (GT_PRI_OVERRIDE_FTYPE)
*
* OUTPUTS:
*       entry - Q Priority Override Table entry (GT_QPRI_TBL_ENTRY)
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM     - on unknown frame type
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gsysGetQPriAvbOverrideTable
(
    IN  GT_QD_DEV     *dev,
    IN  GT_PRI_OVERRIDE_FTYPE    fType,
    OUT GT_QPRI_TBL_ENTRY    *entry
);


/* gtPortCtrl.c */

/*******************************************************************************
* gprtGet200Base
*
* DESCRIPTION:
*        200 Base mode. This bit can be used to change the port's Px_GTXCLK
*        frequency to 50MHz to support 200 BASE mode as follows:
*        0 = 25MHz Px_GTXCLK
*        1 = 50MHz Px_GTXCLK
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        mode - 0 for 100Mbps, 1 for 200Mbps
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        C_Mode should be set to 0x2 in order for this API to work
*
*******************************************************************************/
GT_STATUS gprtGet200Base
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_U32      *mode
);


/*******************************************************************************
* gprtSet200Base
*
* DESCRIPTION:
*        200 Base mode. This bit can be used to change the port's Px_GTXCLK
*        frequency to 50MHz to support 200 BASE mode as follows:
*            0 = 25MHz Px_GTXCLK
*            1 = 50MHz Px_GTXCLK
*
* INPUTS:
*        port - the logical port number.
*        mode - 0 for 100Mbps, 1 for 200Mbps
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        C_Mode should be set to 0x2 in order for this API to work
*
*******************************************************************************/
GT_STATUS gprtSet200Base
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_U32      mode
);


/*******************************************************************************
* gprtSetQueueCtrl
*
* DESCRIPTION:
*        Set port queue control data to the Port Queue Control register. 
*        The registers of Port Queue control are.
*         Hard Queue Limits register space
*         Reserved for future Hard Queue Limits use
*
* INPUTS:
*        port  - logical port number
*        point - Pointer to the Port Queue Control register.
*        data  - Port Queue Control data written to the register 
*                pointed to by the point above.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gprtSetQueueCtrl
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT         port,
    IN  GT_U32           point,
    IN  GT_U8            data
);

/*******************************************************************************
* gprtGetQueueCtrl
*
* DESCRIPTION:
*        Get port queue control data from the Port Queue Control register. 
*        The registers of Port Queue control are.
*         Hard Queue Limits register space
*         Reserved for future Hard Queue Limits use
*
* INPUTS:
*        port  - logical port number
*        point - Pointer to the Port Queue Control register.
*
* OUTPUTS:
*        data  - Port Queue Control data written to the register 
*                pointed to by the point above.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gprtGetQueueCtrl
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT         port,
    IN  GT_U32           point,
    OUT GT_U8            *data
);

/*******************************************************************************
* gprtGetDebugCounter
*
* DESCRIPTION:
*        Get Port Debug Counter, bad counter and good counter.
*
* INPUTS:
*        port  - logical port number
*
* OUTPUTS:
*        badCounter  - When CtrMode is cleared to a zero (Global 1 offset 0x1C) this
*  counter increments each time a frame enters this port that was an error on the
*  wire. It does not matter if the frame�s CRC is fixed by ForceGoodFCS (Port
*  offset 0x08) being set to a one, this counter will still increment. A CRC error
*  frame is one that is 64 bytes to MaxFrameSize (Global 1, offset 0x04) with a
*  bad CRC (including alignment errors but not dribbles). Fragments and
*  properly formed frames are not counted. The RxBadFrames counter counts
*  frames that are counted in the MIB counters as InUndersize, InOversize,
*  InJabber, InRxErr and InFCSErr.
*  When CtrMode is set to a one this counter increments each time a transmit
*  collision occurs on this port.
*        goodCounter  - When CtrMode is cleared to a zero (Global 1 offset 0x1C) this
*  counter increments each time a frame enters this port that was not an error
*  frame on the wire. It does not matter if the frame was filtered or discarded,
*  only that the frame was received as good on the wire (i.e., its wire size is in the
*  range of 64 bytes to MaxFrameSize (Global 1, offset 0x04) and its CRC was
*  good. The RxGoodFrames counter counts frames that are not counted
*  above as long as they are not being counted in the MIB counters as
*  InFragments.
*  When CtrMode is set to a one this counter increments each time a frame is
*  transmitted out this port.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gprtGetDebugCounter
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT         port,
    OUT  GT_U8            *badCounter,
    OUT  GT_U8            *goodCounter
);

/*******************************************************************************
* gprtSetCutThrough
*
* DESCRIPTION:
*        Set port Cut Through configuration.
*
* INPUTS:
*        port  - logical port number
*        cutThru - Cut through configuration.
*                    enableSelect;     Port Enable Select. 
*                    enable;           Cut Through enable. 
*                    cutThruQueue;     Cut Through Queues. 
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gprtSetCutThrough
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT         port,
    IN  GT_CUT_THROUGH   *cutThru
);

/*******************************************************************************
* gprtGetCutThrough
*
* DESCRIPTION:
*        Get port Cut Through configuration.
*
* INPUTS:
*        port  - logical port number
*
* OUTPUTS:
*        cutThru - Cut through configuration.
*                    enableSelect;     Port Enable Select. 
*                    enable;           Cut Through enable. 
*                    cutThruQueue;     Cut Through Queues. 
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gprtGetCutThrough
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT         port,
    OUT GT_CUT_THROUGH   *cutThru
);

/*******************************************************************************
* gsysSetDefQpri
*
* DESCRIPTION:
*       This routine Set Default queue priority.
*
* INPUTS:
*       port - the logical port number
*       defque - default queue priority structure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gsysSetDefQpri
(
  IN  GT_QD_DEV    *dev,
  IN  GT_LPORT    port,
  IN  GT_DEF_QUE_PRI  *defque
);

/*******************************************************************************
* gsysGetDefQpri
*
* DESCRIPTION:
*       This routine Get Default queue priority.
*
* INPUTS:
*       port - the logical port number
*
* OUTPUTS:
*       defque - default queue priority structure
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gsysGetDefQpri
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_DEF_QUE_PRI  *defque
);

/*******************************************************************************
* gprtSetAllowVid0
*
* DESCRIPTION:
*        Allow VID of Zero.
*
* INPUTS:
*        port - the logical port number.
*        allowed - GT_TRUE, Allow frames received with a VID of 0x000 
*                  will NOT be updated with the port�s Default VID.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetAllowVid0
(
    IN GT_QD_DEV       *dev,
    IN GT_LPORT        port,
    IN GT_BOOL         allowed
);

/*******************************************************************************
* gprtGetAllowVid0
*
* DESCRIPTION:
*        This routine gets Ignore FCS setup
*
* DESCRIPTION:
*        Allow VID of Zero.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        allowed - GT_TRUE, Allow frames received with a VID of 0x000 
*                  will NOT be updated with the port�s Default VID.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetAllowVid0
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *allowed
);

/*******************************************************************************
* gportPointDataSet
*
* DESCRIPTION:
*        Common set indirect register pointer control data from the port 
*        function registers. The function register are.
*          Flow Control
*
* INPUTS:
*       port  - the logical port number.
*        funcReg - port function register.
*        point - Pointer to following Control register.
*          Flow Control
*        data  - Scratch and Misc. Control data written to the register 
*                pointed to by the point above.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gportPointDataSet
(
    IN  GT_QD_DEV         *dev,
    IN  GT_LPORT   port,
    IN  GT_U8             funcReg,
    IN  GT_U8            point,
    IN  GT_U8            data
);

/*******************************************************************************
* gportPointDataGet
*
* DESCRIPTION:
*        Common get indirect register pointer control data from the port 
*        function registers. The function register are.
*          Flow Control
*
* INPUTS:
*       port  - the logical port number.
*        funcReg - port function register.
*        point - Pointer to following Control register.
*          Flow Control
*
* OUTPUTS:
*        data -  Control data read from the indirect register 
*                pointed to by the point above.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gportPointDataGet
(
    IN   GT_QD_DEV         *dev,
    IN   GT_LPORT   port,
    IN   GT_U8            funcReg,
    IN   GT_U8            point,
    OUT  GT_U8            *data
);

/*******************************************************************************
* gportSetFlowCtrl
*
* DESCRIPTION:
*        Set Flow control data from the Flow Control register. 
*        The register of Flow control are:
*          Port Limit In 
*          Port Limit Out 
*          Port Flow Control Mode 
*          IEEE 802.3x Queues to Pause 
*          PFC Priority to Tx Queue Mapping 
*          Rx Queue to PFC Priority Mapping 
*         
* INPUTS:
*        point - Pointer to the Flow control register.
*        data  - Flow Control data written to the register 
*                pointed to by the point above.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gportSetFlowCtrl
(
    IN  GT_QD_DEV         *dev,
    IN  GT_LPORT          port,
    IN  GT_U8             point,
    IN  GT_U8            data
);

/*******************************************************************************
* gportGetFlowCtrl
*
* DESCRIPTION:
*        Get Flow control data from the Flow Control register. 
*        The register of Flow control are:
*          Port Limit In 
*          Port Limit Out 
*          Port Flow Control Mode 
*          IEEE 802.3x Queues to Pause 
*          PFC Priority to Tx Queue Mapping 
*          Rx Queue to PFC Priority Mapping 
*         
* INPUTS:
*        point - Pointer to the Flow control register.
*
* OUTPUTS:
*        data  - Flow Control data written to the register 
*                pointed to by the point above.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gportGetFlowCtrl
(
    IN  GT_QD_DEV         *dev,
    IN  GT_LPORT          port,
    IN  GT_U8             point,
    OUT  GT_U8            *data
);

/*******************************************************************************
* gprtSetPortCtrl3
*
* DESCRIPTION:
*        Set port control features in port control 3 .
*
* INPUTS:
*        port - the logical port number.
*        control - Combine Following control bits.
*         GT_PORT_CONTROL_3_USECFI_YELLOW 
*         GT_PORT_CONTROL_3_USEDEI_YELLOW   
*         GT_PORT_CONTROL_3_UPDATECFI_YELLOW
*         GT_PORT_CONTROL_3_UPDATEDEI_YELLOW 
*         GT_PORT_CONTROL_3_USEIP_YELLOW
*		  GT_PORT_CONTROL_3_IPAVB_EN
*		  GT_PORT_CONTROL_3_DROP_BC
*		  GT_PORT_CONTROL_3_PAV_4_INT_CPU
*		  GT_PORT_CONTROL_3_MAP_2_INT_CPU   
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetPortCtrl3
(
    IN GT_QD_DEV       *dev,
    IN GT_LPORT        port,
    IN GT_U16         control
);

/*******************************************************************************
* gprtGetPortCtrl3
*
* DESCRIPTION:
*        This routine gets Ignore FCS setup
*
* DESCRIPTION:
*        Allow VID of Zero.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        control - Combine Following control bits.
*         GT_PORT_CONTROL_3_USECFI_YELLOW 
*         GT_PORT_CONTROL_3_USEDEI_YELLOW   
*         GT_PORT_CONTROL_3_UPDATECFI_YELLOW
*         GT_PORT_CONTROL_3_UPDATEDEI_YELLOW 
*         GT_PORT_CONTROL_3_USEIP_YELLOW
*		  GT_PORT_CONTROL_3_IPAVB_EN
*		  GT_PORT_CONTROL_3_DROP_BC
*		  GT_PORT_CONTROL_3_PAV_4_INT_CPU
*		  GT_PORT_CONTROL_3_MAP_2_INT_CPU    
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetPortCtrl3
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_U16        *control
);

/*******************************************************************************
* gportSetQueueCtrl
*
* DESCRIPTION:
*        Set Queue control data to the Queue Control register. 
*        The register(pointer) of Queue control are:
*            GT_QUEUE_CFG_PORT_SCHEDULE
*            GT_QUEUE_CFG_FILTER_Q_EN
*            GT_QUEUE_CFG_PORT_SCRATCH_REG,     (acratch0 - 1 inc: 1)
*            GT_QUEUE_CFG_DANAMIC_Q_PRUNNING
*            GT_QUEUE_CFG_DANAMIC_Q_STATUS
*            GT_QUEUE_CFG_DANAMIC_Q_YELLOW_CTRL
*            GT_QUEUE_CFG_H_Q_LIMIT_REG         (Q0 - Q7 inc: 1)
*            GT_QUEUE_CFG_Y_Q_LIMIT_REG         (Q0 - Q7 inc: 1)
*            GT_QUEUE_CFG_PFC_THRESHOLD_BEGIN   (Q0 - Q7 inc: 2)
*            GT_QUEUE_CFG_PFC_THRESHOLD_END     (Q0 - Q7 inc: 2)
*            GT_QUEUE_CFG_IN_Q_COUNT            (Q0 - Q7 inc: 1)
*            GT_QUEUE_CFG_QOS_GAIN_SET
*            GT_QUEUE_CFG_PFC_GAIN_SET
*         
* INPUTS:
*        point - Pointer to the Queue control register.
*        data  - Queue Control data written to the register 
*                pointed to by the point above.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gportSetQueueCtrl
(
    IN  GT_QD_DEV         *dev,
    IN  GT_LPORT          port,
    IN  GT_U8             point,
    IN  GT_U8            data
);

/*******************************************************************************
* gportGetQueueCtrl
*
* DESCRIPTION:
*        Get Queue control data from the Queue Control register. 
*        The register(pointer) of Queue control are:
*            GT_QUEUE_CFG_PORT_SCHEDULE
*            GT_QUEUE_CFG_FILTER_Q_EN
*            GT_QUEUE_CFG_PORT_SCRATCH_0_REG,  (0 - 1)
*            GT_QUEUE_CFG_DANAMIC_Q_PRUNNING
*            GT_QUEUE_CFG_DANAMIC_Q_STATUS
*            GT_QUEUE_CFG_DANAMIC_Q_YELLOW_CTRL
*            GT_QUEUE_CFG_H_Q_LIMIT_REG         (0 - 7)
*            GT_QUEUE_CFG_Y_Q_LIMIT_REG         (0 - 7)
*            GT_QUEUE_CFG_PFC_THRESHOLD_BEGIN   (0 - 7, increment=2)
*            GT_QUEUE_CFG_PFC_THRESHOLD_END     (0 - 7, increment=2)
*            GT_QUEUE_CFG_IN_Q_COUNT            (0 - 7)
*            GT_QUEUE_CFG_QOS_GAIN_SET
*            GT_QUEUE_CFG_PFC_GAIN_SET
*         
* INPUTS:
*        point - Pointer to the Queue control register.
*
* OUTPUTS:
*        data  - Queue Control data written to the register 
*                pointed to by the point above.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gportGetQueueCtrl
(
    IN  GT_QD_DEV         *dev,
    IN  GT_LPORT          port,
    IN  GT_U8             point,
    OUT  GT_U8            *data
);

/* gtPTP.c */

/*******************************************************************************
* gtaiSetEventConfig
*
* DESCRIPTION:
*       This routine sets TAI Event Capture configuration parameters.
*
* INPUTS:
*        eventData  - TAI Event Capture configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiSetEventConfig
(
    IN  GT_QD_DEV     *dev,
    IN  GT_TAI_EVENT_CONFIG    *eventData
);


/*******************************************************************************
* gtaiGetEventConfig
*
* DESCRIPTION:
*       This routine gets TAI Event Capture configuration parameters.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        eventData  - TAI Event Capture configuration parameters.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiGetEventConfig
(
    IN  GT_QD_DEV     *dev,
    OUT GT_TAI_EVENT_CONFIG    *eventData
);

/*******************************************************************************
* gtaiGetEventStatus
*
* DESCRIPTION:
*       This routine gets TAI Event Capture status.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        eventData  - TAI Event Capture configuration parameters.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiGetEventStatus
(
    IN  GT_QD_DEV     *dev,
    OUT GT_TAI_EVENT_STATUS    *status
);

/*******************************************************************************
* gtaiGetEventInt
*
* DESCRIPTION:
*       This routine gets TAI Event Capture Interrupt status.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        intStatus     - interrupt status for TAI Event capture
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiGetEventInt
(
    IN  GT_QD_DEV     *dev,
    OUT GT_BOOL        *intStatus
);

/*******************************************************************************
* gtaiClearEventInt
*
* DESCRIPTION:
*       This routine clear TAI Event Capture Interrupt status.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiClearEventInt
(
    IN  GT_QD_DEV     *dev
);

/*******************************************************************************
* gtaiSetClockSelect
*
* DESCRIPTION:
*       This routine sets several clock select in TAI.
*
* INPUTS:
*        clkSelect  - TAI clock select configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiSetClockSelect
(
    IN  GT_QD_DEV     *dev,
    IN  GT_TAI_CLOCK_SELECT    *clkSelect
);

/*******************************************************************************
* gtaiGetClockSelect
*
* DESCRIPTION:
*       This routine gets several clock select in TAI.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       clkSelect  - TAI clock select configuration parameters.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiGetClockSelect
(
    IN  GT_QD_DEV     *dev,
    OUT  GT_TAI_CLOCK_SELECT    *clkSelect
);

/*******************************************************************************
* gtaiSetClockConfig
*
* DESCRIPTION:
*       This routine sets clock configuration in TAI.
*
* INPUTS:
*        clkConfig  - TAI clock configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiSetClockConfig
(
    IN  GT_QD_DEV     *dev,
    IN  GT_TAI_CLOCK_CONFIG    *clkConfig
);

/*******************************************************************************
* gtaiGetClockConfig
*
* DESCRIPTION:
*       This routine gets clock configuration in TAI.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       clkConfig  - TAI clock configuration parameters.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiGetClockConfig
(
    IN  GT_QD_DEV     *dev,
    OUT  GT_TAI_CLOCK_CONFIG    *clkConfig
);

/*******************************************************************************
* gtaiGetTrigInt
*
* DESCRIPTION:
*       This routine gets TAI Trigger Interrupt status.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        intStatus     - interrupt status for TAI Trigger
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiGetTrigInt
(
    IN  GT_QD_DEV     *dev,
    OUT GT_BOOL        *intStatus
);

/*******************************************************************************
* gtaiClearTrigInt
*
* DESCRIPTION:
*       This routine clear TAI Trigger Interrupt status.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiClearTrigInt
(
    IN  GT_QD_DEV     *dev
);

/*******************************************************************************
* gtaiSetTrigConfig
*
* DESCRIPTION:
*       This routine sets TAI Trigger configuration parameters.
*
* INPUTS:
*        trigEn    - enable/disable TAI Trigger.
*        trigData  - TAI Trigger configuration parameters (valid only if trigEn is GT_TRUE).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiSetTrigConfig
(
    IN  GT_QD_DEV     *dev,
    IN  GT_BOOL     trigEn,
    IN  GT_TAI_TRIGGER_CONFIG    *trigData
);

/*******************************************************************************
* gtaiGetTrigConfig
*
* DESCRIPTION:
*       This routine gets TAI Trigger configuration parameters.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        trigEn    - enable/disable TAI Trigger.
*        trigData  - TAI Trigger configuration parameters (valid only if trigEn is GT_TRUE).
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiGetTrigConfig
(
    IN  GT_QD_DEV     *dev,
    OUT GT_BOOL     *trigEn,
    OUT GT_TAI_TRIGGER_CONFIG    *trigData
);


/*******************************************************************************
* gtaiSetTrigLock
*
* DESCRIPTION:
*       This routine sets TAI Trigger lock.
*
* INPUTS:
*       trigLockCfg - trig lock configuration:
*        trigLock       - trigger lock set.
*        trigLockRange  - trigger lock range.
*        lockCorrect    -  Trig Lock Correction amount 
*        lockCorrect2    - Trig Lock Correction amount 2
*        trigGenTime     - Trigger Generation Time. 
*        trigGen2Time    - Trigger Generation Time 2. 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiSetTrigLock
(
    IN  GT_QD_DEV     *dev,
    INOUT  GT_TAI_TRIG_LOCK_CONFIG    *trigLockCfg
);

/*******************************************************************************
* gtaiGetTrigLock
*
* DESCRIPTION:
*       This routine gets TAI Trigger lock and trigger lock range.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       trigLockCfg - trig lock configuration:
*        trigLock       - trigger lock set.
*        trigLockRange  - trigger lock range.
*        lockCorrect    -  Trig Lock Correction amount 
*        lockCorrect2    - Trig Lock Correction amount 2
*        trigGenTime     - Trigger Generation Time. 
*        trigGen2Time    - Trigger Generation Time 2. 
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiGetTrigLock
(
    IN  GT_QD_DEV     *dev,
    INOUT  GT_TAI_TRIG_LOCK_CONFIG    *trigLockCfg
);

/*******************************************************************************
* gtaiGetTSClkPer
*
* DESCRIPTION:
*         Time Stamping Clock Period in pico seconds.
*        This routine specifies the clock period for the time stamping clock supplied 
*        to the PTP hardware logic.
*        This is the clock that is used by the hardware logic to update the PTP 
*        Global Time Counter.
*
* INPUTS:
*         None.
*
* OUTPUTS:
*        clk        - time stamping clock period
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gtaiGetTSClkPer
(
    IN  GT_QD_DEV     *dev,
    OUT GT_U32        *clk
);


/*******************************************************************************
* gtaiSetTSClkPer
*
* DESCRIPTION:
*         Time Stamping Clock Period in pico seconds.
*        This routine specifies the clock period for the time stamping clock supplied 
*        to the PTP hardware logic.
*        This is the clock that is used by the hardware logic to update the PTP 
*        Global Time Counter.
*
* INPUTS:
*        clk        - time stamping clock period
*
* OUTPUTS:
*         None.
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gtaiSetTSClkPer
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U32        clk
);

/*******************************************************************************
* gtaiSetMultiPTPSync
*
* DESCRIPTION:
*         This routine sets Multiple PTP device sync mode and sync time (TrigGenAmt).
*        When enabled, the hardware logic detects a low to high transition on the 
*        EventRequest(GPIO) and transfers the sync time into the PTP Global Time
*        register. The EventCapTime is also updated at that instant.
*
* INPUTS:
*        multiEn        - enable/disable Multiple PTP device sync mode
*        syncTime    - sync time (valid only if multiEn is GT_TRUE)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         When enabled, gtaiSetTrigConfig, gtaiSetEventConfig, gtaiSetTimeInc,
*        and gtaiSetTimeDec APIs are not operational.
*
*******************************************************************************/
GT_STATUS gtaiSetMultiPTPSync
(
    IN  GT_QD_DEV     *dev,
    IN  GT_BOOL     multiEn,
    IN  GT_32        syncTime
);

/*******************************************************************************
* gtaiGetMultiPTPSync
*
* DESCRIPTION:
*         This routine gets Multiple PTP device sync mode and sync time (TrigGenAmt).
*        When enabled, the hardware logic detects a low to high transition on the 
*        EventRequest(GPIO) and transfers the sync time into the PTP Global Time
*        register. The EventCapTime is also updated at that instant.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        multiEn        - enable/disable Multiple PTP device sync mode
*        syncTime    - sync time (valid only if multiEn is GT_TRUE)
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         When enabled, gtaiSetTrigConfig, gtaiSetEventConfig, gtaiSetTimeInc,
*        and gtaiSetTimeDec APIs are not operational.
*
*******************************************************************************/
GT_STATUS gtaiGetMultiPTPSync
(
    IN  GT_QD_DEV     *dev,
    OUT GT_BOOL     *multiEn,
    OUT GT_32        *syncTime
);

/*******************************************************************************
* gtaiGetTimeIncDec
*
* DESCRIPTION:
*         This routine retrieves Time increment/decrement setup.
*        This amount specifies the number of units of PTP Global Time that need to be 
*        incremented or decremented. This is used for adjusting the PTP Global Time 
*        counter value by a certain amount.
*
* INPUTS:
*         None.
*
* OUTPUTS:
*        en        - GT_TRUE if enabled, GT_FALSE otherwise
*        inc        - GT_TRUE if increment, GT_FALSE if decrement
*        amount    - increment/decrement amount
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gtaiGetTimeIncDec
(
    IN  GT_QD_DEV     *dev,
    OUT GT_BOOL        *en,
    OUT GT_BOOL        *inc,
    OUT GT_U32        *amount
);

/*******************************************************************************
* gtaiSetTimeInc
*
* DESCRIPTION:
*         This routine enables time increment by the specifed time increment amount.
*        The amount specifies the number of units of PTP Global Time that need to be 
*        incremented. This is used for adjusting the PTP Global Time counter value by
*        a certain amount.
*        Increment occurs just once.
*
* INPUTS:
*        amount    - time increment amount (0 ~ 0xFF)
*
* OUTPUTS:
*         None.
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gtaiSetTimeInc
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U32        amount
);

/*******************************************************************************
* gtaiSetTimeDec
*
* DESCRIPTION:
*         This routine enables time decrement by the specifed time decrement amount.
*        The amount specifies the number of units of PTP Global Time that need to be 
*        decremented. This is used for adjusting the PTP Global Time counter value by
*        a certain amount.
*        Decrement occurs just once.
*
* INPUTS:
*        amount    - time decrement amount (0 ~ 0x7FF)
*
* OUTPUTS:
*         None.
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gtaiSetTimeDec
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U32        amount
);

/*******************************************************************************
* gavbGetReg
*
* DESCRIPTION:
*       This routine reads AVB register.
*
* INPUTS:
*       port         - logical port number.
*       regOffset    - register to read
*
* OUTPUTS:
*        data        - register data
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gavbGetReg
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_U32        regOffset,
    OUT GT_U32        *data
);

/*******************************************************************************
* gavbSetReg
*
* DESCRIPTION:
*       This routine writes data to AVB register.
*
* INPUTS:
*       port         - logical port number
*       regOffset    - register to be written
*        data        - data to be written
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gavbSetReg
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_U32        regOffset,
    IN  GT_U32        data
);

/*******************************************************************************
* gavbGetPriority
*
* DESCRIPTION:
*        Priority overwrite.
*        Supported priority type is defined as GT_AVB_PRI_TYPE.
*        Priority is either 3 bits or 2 bits depending on priority type.
*            GT_AVB_HI_FPRI        - priority is 0 ~ 7
*            GT_AVB_HI_QPRI        - priority is 0 ~ 3(after Opus is 7)
*            GT_AVB_LO_FPRI        - priority is 0 ~ 7
*            GT_AVB_LO_QPRI        - priority is 0 ~ 3(after Opus is 7)
*            GT_LEGACY_HI_FPRI    - priority is 0 ~ 7
*            GT_LEGACY_HI_QPRI    - priority is 0 ~ 3(after Opus is 7)
*            GT_LEGACY_LO_FPRI    - priority is 0 ~ 7
*            GT_LEGACY_LO_QPRI    - priority is 0 ~ 3(after Opus is 7)
*            (after Opus support Midium priorities)
*
* INPUTS:
*         priType    - GT_AVB_PRI_TYPE
*
* OUTPUTS:
*        pri    - priority
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gavbGetPriority
(
    IN  GT_QD_DEV     *dev,
    IN  GT_AVB_PRI_TYPE        priType,
    OUT GT_U8        *pri
);

/*******************************************************************************
* gavbSetPriority
*
* DESCRIPTION:
*        Priority overwrite.
*        Supported priority type is defined as GT_AVB_PRI_TYPE.
*        Priority is either 3 bits or 2 bits depending on priority type.
*            GT_AVB_HI_FPRI        - priority is 0 ~ 7
*            GT_AVB_HI_QPRI        - priority is 0 ~ 3(after Opus is 7)
*            GT_AVB_LO_FPRI        - priority is 0 ~ 7
*            GT_AVB_LO_QPRI        - priority is 0 ~ 3(after Opus is 7)
*            GT_LEGACY_HI_FPRI    - priority is 0 ~ 7
*            GT_LEGACY_HI_QPRI    - priority is 0 ~ 3(after Opus is 7)
*            GT_LEGACY_LO_FPRI    - priority is 0 ~ 7
*            GT_LEGACY_LO_QPRI    - priority is 0 ~ 3(after Opus is 7)
*            (after Opus support Midium priorities)
*
* INPUTS:
*         priType    - GT_AVB_PRI_TYPE
*        pri    - priority
*
* OUTPUTS:
*        None.
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gavbSetPriority
(
    IN  GT_QD_DEV     *dev,
    IN  GT_AVB_PRI_TYPE        priType,
    IN  GT_U8        pri
);

/*******************************************************************************
* gavbGetMidPriEn
*
* DESCRIPTION:
*        Get Medium priority enable.
*
* INPUTS:
*         None
*
* OUTPUTS:
*        enHi    - GT_TRUE if medium high priority enabled, GT_FALSE otherwise
*        enLo    - GT_TRUE if medium low priority enabled, GT_FALSE otherwise
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gavbGetMidPriEn
(
    IN  GT_QD_DEV     *dev,
    OUT GT_BOOL        *enHi,
    OUT GT_BOOL        *enLo
);

/*******************************************************************************
* gavbSetMidPriEn
*
* DESCRIPTION:
*        Set Medium priority enable.
*
* INPUTS:
*        enHi    - GT_TRUE if medium high priority enabled, GT_FALSE otherwise
*        enLo    - GT_TRUE if medium low priority enabled, GT_FALSE otherwise
*
* OUTPUTS:
*         None
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gavbSetMidPriEn
(
    IN  GT_QD_DEV     *dev,
    IN  GT_BOOL        enHi,
    IN  GT_BOOL        enLo
);

/*******************************************************************************
* gavbGetAVBHiLimit
*
* DESCRIPTION:
*        AVB Hi Frame Limit.
*        When these bits are zero, normal frame processing occurs. 
*        When it's non-zero, they are used to define the maximum frame size allowed
*        for AVB frames that can be placed into the GT_AVB_HI_QPRI queue. Frames
*        that are over this size limit are filtered. The only exception to this 
*        is non-AVB frames that get their QPriAvb assigned by the Priority Override 
*        Table
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        limit    - Hi Frame Limit
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gavbGetAVBHiLimit
(
    IN  GT_QD_DEV     *dev,
    OUT GT_U32        *limit
);

/*******************************************************************************
* gavbSetAVBHiLimit
*
* DESCRIPTION:
*        AVB Hi Frame Limit.
*        When these bits are zero, normal frame processing occurs. 
*        When it's non-zero, they are used to define the maximum frame size allowed
*        for AVB frames that can be placed into the GT_AVB_HI_QPRI queue. Frames
*        that are over this size limit are filtered. The only exception to this 
*        is non-AVB frames that get their QPriAvb assigned by the Priority Override 
*        Table
*
* INPUTS:
*        limit    - Hi Frame Limit
*
* OUTPUTS:
*        None.
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gavbSetAVBHiLimit
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U32        limit
);

/*******************************************************************************
* gavbGetPtpExtClk
*
* DESCRIPTION:
*        PTP external clock select.
*        When this bit is cleared to a zero, the PTP core gets its clock from 
*        an internal 125MHz clock based on the device's XTAL_IN input. 
*        When this bit is set to a one, the PTP core gets its clock from the device's
*        PTP_EXTCLK pin.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        extClk    - GT_TRUE if external clock is selected, GT_FALSE otherwise
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gavbGetPtpExtClk
(
    IN  GT_QD_DEV     *dev,
    OUT GT_BOOL        *extClk
);

/*******************************************************************************
* gavbSetPtpExtClk
*
* DESCRIPTION:
*        PTP external clock select.
*        When this bit is cleared to a zero, the PTP core gets its clock from 
*        an internal 125MHz clock based on the device's XTAL_IN input. 
*        When this bit is set to a one, the PTP core gets its clock from the device's
*        PTP_EXTCLK pin.
*
* INPUTS:
*        extClk    - GT_TRUE if external clock is selected, GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gavbSetPtpExtClk
(
    IN  GT_QD_DEV     *dev,
    IN  GT_BOOL        extClk
);

/*******************************************************************************
* gavbGetRecClkSel
*
* DESCRIPTION:
*        Synchronous Ethernet Recovered Clock Select.
*        This field indicate the internal PHY number whose recovered clock will
*        be presented on the SE_RCLK0 or SE_RCLK1 pin depending on the recClk selection.
*
* INPUTS:
*        recClk    - GT_AVB_RECOVERED_CLOCK type
*
* OUTPUTS:
*        clkSel    - recovered clock selection
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gavbGetRecClkSel
(
    IN  GT_QD_DEV     *dev,
    IN  GT_AVB_RECOVERED_CLOCK    recClk,
    OUT GT_U32        *clkSel
);


/*******************************************************************************
* gavbSetRecClkSel
*
* DESCRIPTION:
*        Synchronous Ethernet Recovered Clock Select.
*        This field indicate the internal PHY number whose recovered clock will
*        be presented on the SE_RCLK0 or SE_RCLK1 pin depending on the recClk selection.
*
* INPUTS:
*        recClk    - GT_AVB_RECOVERED_CLOCK type
*        clkSel    - recovered clock selection (should be less than 8)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gavbSetRecClkSel
(
    IN  GT_QD_DEV     *dev,
    IN  GT_AVB_RECOVERED_CLOCK    recClk,
    IN  GT_U32        clkSel
);

/*******************************************************************************
* gavbGetAvbOuiBytes
*
* DESCRIPTION:
*        AVB OUI Limit Filter bytes(0 ~ 2).
*        When all three of the AvbOui Bytes are zero, normal frame processing occurs.
*        When any of the three AvbOui Bytes are non-zero, all AVB frames must have a
*        destination address whose 1st three bytes of the DA match these three 
*        AvbOui Bytes or the frame will be filtered.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        ouiBytes    - 3 bytes of OUI field in Ethernet address format
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gavbGetAvbOuiBytes
(
    IN  GT_QD_DEV     *dev,
    OUT GT_U8        *obiBytes
);

/*******************************************************************************
* gavbSetAvbOuiBytes
*
* DESCRIPTION:
*        AVB OUI Limit Filter bytes(0 ~ 2).
*        When all three of the AvbOui Bytes are zero, normal frame processing occurs.
*        When any of the three AvbOui Bytes are non-zero, all AVB frames must have a
*        destination address whose 1st three bytes of the DA match these three 
*        AvbOui Bytes or the frame will be filtered.
*
* INPUTS:
*        ouiBytes    - 3 bytes of OUI field in Ethernet address format
*
* OUTPUTS:
*        None.
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gavbSetAvbOuiBytes
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U8        *obiBytes
);

/*******************************************************************************
* gavbGetAvbMode
*
* DESCRIPTION:
*        Port's AVB Mode.
*
* INPUTS:
*        port    - the logical port number
*
* OUTPUTS:
*        mode    - GT_AVB_MODE type
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gavbGetAvbMode
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    OUT GT_AVB_MODE    *mode
);

/*******************************************************************************
* gavbSetAvbMode
*
* DESCRIPTION:
*        Port's AVB Mode.
*
* INPUTS:
*        port    - the logical port number
*        mode    - GT_AVB_MODE type
*
* OUTPUTS:
*        None.
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gavbSetAvbMode
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    IN  GT_AVB_MODE    mode
);

/*******************************************************************************
* gavbGetAvbOverride
*
* DESCRIPTION:
*        AVB Override.
*        When disabled, normal frame processing occurs.
*        When enabled, the egress portion of this port is considered AVB even if 
*        the ingress portion is not.
*
* INPUTS:
*        port    - the logical port number
*
* OUTPUTS:
*        en        - GT_TRUE if enabled, GT_FALSE otherwise
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gavbGetAvbOverride
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    OUT GT_BOOL        *en
);

/*******************************************************************************
* gavbSetAvbOverride
*
* DESCRIPTION:
*        AVB Override.
*        When disabled, normal frame processing occurs.
*        When enabled, the egress portion of this port is considered AVB even if 
*        the ingress portion is not.
*
* INPUTS:
*        port    - the logical port number
*        en        - GT_TRUE to enable, GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gavbSetAvbOverride
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    IN  GT_BOOL        en
);


/*******************************************************************************
* gavbGetFilterBadAvb
*
* DESCRIPTION:
*        Filter Bad AVB frames.
*        When disabled, normal frame processing occurs.
*        When enabled, frames that are considered Bad AVB frames are filtered.
*
* INPUTS:
*        port    - the logical port number
*
* OUTPUTS:
*        en        - GT_TRUE if enabled, GT_FALSE otherwise
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gavbGetFilterBadAvb
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    OUT GT_BOOL        *en
);

/*******************************************************************************
* gavbSetFilterBadAvb
*
* DESCRIPTION:
*        Filter Bad AVB frames.
*        When disabled, normal frame processing occurs.
*        When enabled, frames that are considered Bad AVB frames are filtered.
*
* INPUTS:
*        port    - the logical port number
*        en        - GT_TRUE to enable, GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gavbSetFilterBadAvb
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    IN  GT_BOOL        en
);


/*******************************************************************************
* gavbGetAvbTunnel
*
* DESCRIPTION:
*        AVB Tunnel.
*        When disabled, normal frame processing occurs.
*        When enabled, the port based VLAN Table masking, 802.1Q VLAN membership 
*        masking and the Trunk Masking is bypassed for any frame entering this port
*        that is considered AVB by DA. This includes unicast as well as multicast
*        frame
*
* INPUTS:
*        port    - the logical port number
*
* OUTPUTS:
*        en        - GT_TRUE if enabled, GT_FALSE otherwise
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gavbGetAvbTunnel
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    OUT GT_BOOL        *en
);


/*******************************************************************************
* gavbSetAvbTunnel
*
* DESCRIPTION:
*        AVB Tunnel.
*        When disabled, normal frame processing occurs.
*        When enabled, the port based VLAN Table masking, 802.1Q VLAN membership 
*        masking and the Trunk Masking is bypassed for any frame entering this port
*        that is considered AVB by DA. This includes unicast as well as multicast
*        frame
*
* INPUTS:
*        port    - the logical port number
*        en        - GT_TRUE to enable, GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gavbSetAvbTunnel
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    IN  GT_BOOL        en
);


/*******************************************************************************
* gavbGetAvbFramePolicy
*
* DESCRIPTION:
*        AVB Hi or Lo frame policy mapping.
*        Supported policies are defined in GT_AVB_FRAME_POLICY.
*            
* INPUTS:
*        port    - the logical port number
*        fType    - GT_AVB_FRAME_TYPE 
*
* OUTPUTS:
*        policy    - GT_AVB_FRAME_POLICY
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gavbGetAvbFramePolicy
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    IN    GT_AVB_FRAME_TYPE    fType,
    OUT GT_AVB_FRAME_POLICY        *policy
);


/*******************************************************************************
* gavbSetAvbFramePolicy
*
* DESCRIPTION:
*        AVB Hi or Lo frame policy mapping.
*        Supported policies are defined in GT_AVB_FRAME_POLICY.
*
* INPUTS:
*        port    - the logical port number
*        fType    - GT_AVB_FRAME_TYPE 
*        policy    - GT_AVB_FRAME_POLICY
*
* OUTPUTS:
*        None.
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gavbSetAvbFramePolicy
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    IN    GT_AVB_FRAME_TYPE    fType,
    IN  GT_AVB_FRAME_POLICY        policy
);

/*******************************************************************************
* gavbGetAvbPolicy
*
* DESCRIPTION:
*        Get port AVB Policy data.
*
* INPUTS:
*        port    - the logical port number
*
* OUTPUTS:
*        avbPolicy  - GT_AVB_POLICY_STRUCT
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gavbGetAvbPolicy
(
    IN  GT_QD_DEV             *dev,
    IN  GT_LPORT              port,
    OUT GT_AVB_POLICY_STRUCT  *avbPolicy
);

/*******************************************************************************
* gavbSetAvbPolicy
*
* DESCRIPTION:
*        Set AVB Policy.
*
* INPUTS:
*        port    - the logical port number
*        en        - GT_TRUE to enable, GT_FALSE otherwise
*
* OUTPUTS:
*        None
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gavbSetAvbPolicy
(
    IN  GT_QD_DEV             *dev,
    IN  GT_LPORT              port,
    IN  GT_AVB_POLICY_STRUCT  *avbPolicy
);

/* Amber QAV API */
/*******************************************************************************/
/* Amber QAV API */
/*******************************************************************************
* gqavSetPortQpriTimeAwareBlock
*
* DESCRIPTION:
*        This routine set Priority Queue 0-3 time aware block enable on a port.
*
* INPUTS:
*        port    - the logical port number
*        queue   - 0 - 3
*        block   - Time aware block: 0 or deblock: 1.
*        en      - Time aware block/deblock enble.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS  gqavSetPortQpriTimeAwareBlock
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    IN  GT_U8        queue,
    IN  GT_U8        block,  
    IN  GT_BOOL      en   
);

/*******************************************************************************
* gqavGetPortQpriTimeAwareBlock
*
* DESCRIPTION:
*        This routine get Priority Queue 0-3 time aware block enable on a port.
*
* INPUTS:
*        port    - the logical port number
*        queue   - 0 - 3
*        block   - Time aware block: 0 or deblock: 1.
*
* OUTPUTS:
*        en      - Time aware block/deblock enble.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS  gqavGetPortQpriTimeAwareBlock
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    IN  GT_U8        queue,
    IN  GT_U8        block,  
    OUT  GT_BOOL     *en   
);

/*******************************************************************************
* gqavSetPortQpriXQTSToken
*
* DESCRIPTION:
*        This routine set Priority Queue 0-3 time slot tokens on a port.
*        The setting value is number of tokens that need to be subtracted at each 
*        QTS interval boundary.
*
* INPUTS:
*        port    - the logical port number
*        queue     - 0 - 3
*        qtsToken - number of tokens.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gqavSetPortQpriXQTSToken
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    IN  GT_U8        queue,
    IN  GT_U16        qtsToken
);

/*******************************************************************************
* gqavGetPortQpriXQTSToken
*
* DESCRIPTION:
*        This routine get Priority Queue 0-3 time slot tokens on a port.
*        The setting value is number of tokens that need to be subtracted at each 
*        QTS interval boundary.
*
* INPUTS:
*        port    - the logical port number
*        queue - 0 - 3
*
* OUTPUTS:
*        qtsToken - number of tokens
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gqavGetPortQpriXQTSToken
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    IN  GT_U8        queue,
    OUT GT_U16        *qtsToken
);

/*******************************************************************************
* gqavSetPortQpriXBurstBytes
*
* DESCRIPTION:
*        This routine set Priority Queue 0-3 Burst Bytes on a port.
*        This value specifies the number of credits in bytes that can be 
*        accumulated when the queue is blocked from sending out a frame due to 
*        higher priority queue frames being sent out.
*
* INPUTS:
*        port    - the logical port number
*        queue - 0 - 3
*        burst - number of credits in bytes .
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavSetPortQpriXBurstBytes
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    IN  GT_U8        queue,
    IN  GT_U16        burst
);

/*******************************************************************************
* gqavGetPortQpriXBurstBytes
*
* DESCRIPTION:
*        This routine get Priority Queue 0-3 Burst Bytes on a port.
*        This value specifies the number of credits in bytes that can be 
*        accumulated when the queue is blocked from sending out a frame due to 
*        higher priority queue frames being sent out.
*
* INPUTS:
*        port    - the logical port number
*        queue    - 0 - 3
*
* OUTPUTS:
*        burst - number of credits in bytes .
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavGetPortQpriXBurstBytes
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    IN  GT_U8        queue,
    OUT GT_U16        *burst
);

/*******************************************************************************
* gqavSetPortQavEnable
*
* DESCRIPTION:
*        This routine set QAV enable status on a port.
*
* INPUTS:
*        port    - the logical port number
*        en        - GT_TRUE: QAV enable, GT_FALSE: QAV disable
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavSetPortQavEnable
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    IN  GT_BOOL        en
);

/*******************************************************************************
* gqavSetPortQpriXRate
*
* DESCRIPTION:
*        This routine set Priority Queue 2-3 rate on a port.
*
* INPUTS:
*        port    - the logical port number
*        queue - 2 - 3
*        rate - number of credits in bytes .
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavSetPortQpriXRate
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    IN  GT_U8        queue,
    IN  GT_U16        rate
);

/*******************************************************************************
* gqavGetPortQpriXRate
*
* DESCRIPTION:
*        This routine get Priority Queue 2-3 rate Bytes on a port.
*
* INPUTS:
*        port    - the logical port number
*        queue    - 2 - 3
*
* OUTPUTS:
*        rate - number of credits in bytes .
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavGetPortQpriXRate
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    IN  GT_U8        queue,
    OUT GT_U16        *rate
);

/*******************************************************************************
* gqavSetPortQpriXHiLimit
*
* DESCRIPTION:
*        This routine set Priority Queue 2-3 HiLimit on a port.
*
* INPUTS:
*        port    - the logical port number
*        queue - 2 - 3
*        hiLimit - number of credits in bytes .
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavSetPortQpriXHiLimit
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    IN  GT_U8        queue,
    IN  GT_U16        hiLimit
);

/*******************************************************************************
* gqavGetPortQpriXHiLimit
*
* DESCRIPTION:
*        This routine get Priority Queue 2-3 HiLimit Bytes on a port.
*
* INPUTS:
*        port    - the logical port number
*        queue    - 2 - 3
*
* OUTPUTS:
*        hiLimit - number of credits in bytes .
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavGetPortQpriXHiLimit
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    IN  GT_U8        queue,
    OUT GT_U16        *hiLimit
);

/*******************************************************************************
* gqavGetPortQavEnable
*
* DESCRIPTION:
*        This routine get QAV enable status on a port.
*
* INPUTS:
*        port    - the logical port number
*
* OUTPUTS:
*        en        - GT_TRUE: QAV enable, GT_FALSE: QAV disable
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavGetPortQavEnable
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    OUT GT_BOOL        *en
);

/*******************************************************************************
* gqavGetPortCurXQToken
*
* DESCRIPTION:
*        This routine get the current token depth of Priority Queue 0-3  rate bucket on a port .
*
* INPUTS:
*        port    - the logical port number
*        queue - 0 - 3
*
* OUTPUTS:
*        curToken - number of current tokens
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gqavGetPortCurXQToken
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    IN  GT_U8        queue,
    OUT GT_U32        *qtsToken
);

/*******************************************************************************
* gqavGetPortXQHeadPacket
*
* DESCRIPTION:
*        This routine get the amount of bytes needed to schedule the packet that
*        is currently present at the head of the Queue 0-3 on a port .
*
* INPUTS:
*        port    - the logical port number
*        queue - 0 - 3
*
* OUTPUTS:
*        packetByte - number of Head Packet
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gqavGetPortXQHeadPacket
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    IN  GT_U8        queue,
    OUT GT_U32        *packetByte
);

/*******************************************************************************/
/* QAV Global registers processing */
/*******************************************************************************
* gqavSetGlobalAdminMGMT
*
* DESCRIPTION:
*        This routine set to accept Admit Management Frames always.
*
* INPUTS:
*        en - GT_TRUE to set MGMT frame accepted always, 
*             GT_FALSE do not set MGMT frame accepted always
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavSetGlobalAdminMGMT
(
    IN  GT_QD_DEV     *dev,
    IN  GT_BOOL        en
);

/*******************************************************************************
* gqavGetGlobalAdminMGMT
*
* DESCRIPTION:
*        This routine get setting of Admit Management Frames always.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        en - GT_TRUE to set MGMT frame accepted always, 
*             GT_FALSE do not set MGMT frame accepted always
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavGetGlobalAdminMGMT
(
    IN  GT_QD_DEV     *dev,
    OUT GT_BOOL        *en
);

/*******************************************************************************
* gqavSetGlobalDisAvbTrans
*
* DESCRIPTION:
*        This routine Disable AVB Transitions.
*
* INPUTS:
*        en - GT_TRUE: AVB transition may lead to out of ordering issues and/or frame drops.
*             GT_FALSE: AVB transition do not result in any out of ordering of packets
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavSetGlobalDisAvbTrans
(
    IN  GT_QD_DEV     *dev,
    IN  GT_BOOL        en
);

/*******************************************************************************
* gqavGetGlobalDisAvbTrans
*
* DESCRIPTION:
*        This routine get Disable AVB Transitions.
* INPUTS:
*        None.
*
* OUTPUTS:
*        en - GT_TRUE: AVB transition may lead to out of ordering issues and/or frame drops.
*             GT_FALSE: AVB transition do not result in any out of ordering of packets
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavGetGlobalDisAvbTrans
(
    IN  GT_QD_DEV     *dev,
    OUT GT_BOOL        *en
);

/*******************************************************************************
* gqavSetGlobalIsoPtrThreshold
*
* DESCRIPTION:
*        This routine set Global Isochronous Queue Pointer Threshold.
*        This field indicates the total number of isochronous pointers 
*        that are reserved for isochronous streams. The value is expected to be 
*        computed in SRP software and programmed into hardware based on the total 
*        aggregate isochronous streams configured to go through this device..
*
* INPUTS:
*        isoPtrs -  total number of isochronous pointers 
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavSetGlobalIsoPtrThreshold
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U16        isoPtrs
);

/*******************************************************************************
* gqavGetGlobalIsoPtrThreshold
*
* DESCRIPTION:
*        This routine get Global Isochronous Queue Pointer Threshold.
*        This field indicates the total number of isochronous pointers 
*        that are reserved for isochronous streams. The value is expected to be 
*        computed in SRP software and programmed into hardware based on the total 
*        aggregate isochronous streams configured to go through this device..
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        isoPtrs -  total number of isochronous pointers 
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavGetGlobalIsoPtrThreshold
(
    IN  GT_QD_DEV     *dev,
    OUT GT_U16        *isoPtrs
);

/*******************************************************************************
* gqavSetGlobalDisQSD4MGMT
*
* DESCRIPTION:
*        This routine set Disable Queue Scheduler Delays for Management frames..
*
* INPUTS:
*        en - GT_TRUE, it indicates to the Queue Controller to disable applying Queue
*            Scheduler Delays and the corresponding rate regulator does not account
*            for MGMT frames through this queue.
*            GT_FALSE, the MGMT frames follow similar rate regulation and delay
*            regulation envelope as specified for the isochronous queue that the
*            MGMT frames are sharing with.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavSetGlobalDisQSD4MGMT
(
    IN  GT_QD_DEV     *dev,
    IN  GT_BOOL        en
);

/*******************************************************************************
* gqavGetGlobalDisQSD4MGMT
*
* DESCRIPTION:
*        This routine Get Disable Queue Scheduler Delays for Management frames..
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        en - GT_TRUE, it indicates to the Queue Controller to disable applying Queue
*            Scheduler Delays and the corresponding rate regulator does not account
*            for MGMT frames through this queue.
*            GT_FALSE, the MGMT frames follow similar rate regulation and delay
*            regulation envelope as specified for the isochronous queue that the
*            MGMT frames are sharing with.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavGetGlobalDisQSD4MGMT
(
    IN  GT_QD_DEV     *dev,
    OUT GT_BOOL        *en
);

/*******************************************************************************
* gqavSetGlobalInterrupt
*
* DESCRIPTION:
*        This routine set QAV interrupt enable, 
*        The QAV interrypts include:
*        [GT_QAV_INT_ENABLE_ENQ_LMT_BIT]      # EnQ Limit Interrupt Enable 
*        [GT_QAV_INT_ENABLE_ISO_DEL_BIT]      # Iso Delay Interrupt Enable   
*        [GT_QAV_INT_ENABLE_ISO_DIS_BIT]      # Iso Discard Interrupt Enable 
*        [GT_QAV_INT_ENABLE_ISO_LIMIT_EX_BIT] # Iso Packet Memory Exceeded 
*                                              Interrupt Enable 
*
* INPUTS:
*        intEn - [GT_QAV_INT_ENABLE_ENQ_LMT_BIT] OR  
*                [GT_QAV_INT_ENABLE_ISO_DEL_BIT] OR    
*                [GT_QAV_INT_ENABLE_ISO_DIS_BIT] OR  
*                [GT_QAV_INT_ENABLE_ISO_LIMIT_EX_BIT]   
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/

/*******************************************************************************
* gqavSetGlobalDisQSD
*
* DESCRIPTION:
*        This routine set Disable Queue Scheduler Delays for Management frames..
*
* INPUTS:
*        en - GT_TRUE, it indicates to the Queue Controller to disable applying Queue
*            Scheduler Delays.
*            GT_FALSE, 
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavSetGlobalDisQSD
(
    IN  GT_QD_DEV     *dev,
    IN  GT_BOOL        en
);

/*******************************************************************************
* gqavGetGlobalDisQSD
*
* DESCRIPTION:
*        This routine Get Disable Queue Scheduler Delays for Management frames..
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        en - GT_TRUE, it indicates to the Queue Controller to disable applying Queue
*            Scheduler Delays.
*            GT_FALSE.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavGetGlobalDisQSD
(
    IN  GT_QD_DEV     *dev,
    OUT GT_BOOL        *en
);
/*******************************************************************************
* gqavSetGlobalDisEnQDelay
*
* DESCRIPTION:
*        This routine set Disable Enqueuing Delay.
*
* INPUTS:
*        en - GT_TRUE,
*            GT_FALSE,
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavSetGlobalDisEnQDelay
(
    IN  GT_QD_DEV     *dev,
    IN  GT_BOOL        en
);

/*******************************************************************************
* gqavGetGlobalDisEnQDelay
*
* DESCRIPTION:
*        This routine Get Disable Enqueuing Delay .
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        en - GT_TRUE, .
*            GT_FALSE, .
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavGetGlobalDisEnQDelay
(
    IN  GT_QD_DEV     *dev,
    OUT GT_BOOL        *en
);

GT_STATUS gqavSetGlobalInterrupt
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U16        intEn
);

/*******************************************************************************
* gqavGetGlobalInterrupt
*
* DESCRIPTION:
*       This routine get QAV interrupt status and enable status, 
*        The QAV interrypt status include:
*         [GT_QAV_INT_STATUS_ENQ_LMT_BIT]      # Enqueue Delay Limit exceeded  
*         [GT_QAV_INT_STATUS_ISO_DEL_BIT]      # Iso Delay Interrupt Status   
*         [GT_QAV_INT_STATUS_ISO_DIS_BIT]      # Iso Discard Interrupt Status 
*         [GT_QAV_INT_STATUS_ISO_LIMIT_EX_BIT] # Iso Packet Memory Exceeded 
*                                                Interrupt Status 
*        The QAV interrypt enable status include:
*         [GT_QAV_INT_ENABLE_ENQ_LMT_BIT]      # EnQ Limit Interrupt Enable 
*         [GT_QAV_INT_ENABLE_ISO_DEL_BIT]      # Iso Delay Interrupt Enable   
*         [GT_QAV_INT_ENABLE_ISO_DIS_BIT]      # Iso Discard Interrupt Enable 
*         [GT_QAV_INT_ENABLE_ISO_LIMIT_EX_BIT] # Iso Packet Memory Exceeded 
*                                                  Interrupt Enable 
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        intEnSt - [GT_QAV_INT_STATUS_ENQ_LMT_BIT] OR     
*                [GT_QAV_INT_STATUS_ISO_DEL_BIT] OR   
*                [GT_QAV_INT_STATUS_ISO_DIS_BIT] OR   
*                [GT_QAV_INT_STATUS_ISO_LIMIT_EX_BIT] OR
*                [GT_QAV_INT_ENABLE_ENQ_LMT_BIT] OR  
*                [GT_QAV_INT_ENABLE_ISO_DEL_BIT] OR    
*                [GT_QAV_INT_ENABLE_ISO_DIS_BIT] OR  
*                [GT_QAV_INT_ENABLE_ISO_LIMIT_EX_BIT]   
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavGetGlobalInterrupt
(
    IN  GT_QD_DEV     *dev,
    OUT GT_U16        *intEnSt
);

/*******************************************************************************
* gqavGetGlobalIsoInterruptPort
*
* DESCRIPTION:
*        This routine get Isochronous interrupt port.
*        This field indicates the port number for IsoDisInt or IsoLimitExInt 
*        bits. Only one such interrupt condition can be detected by hardware at one 
*        time. Once an interrupt bit has been set along with the IsoIntPort, the 
*        software would have to come and clear the bits before hardware records 
*        another interrupt event.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        port - port number for IsoDisInt or IsoLimitExInt bits.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavGetGlobalIsoInterruptPort
(
    IN  GT_QD_DEV     *dev,
    OUT GT_U8        *port
);

/*******************************************************************************
* gqavSetGlobalIsoDelayLmt
*
* DESCRIPTION:
*        This routine set Isochronous queue delay Limit
*        This field represents a per-port isochronous delay limit that 
*        will be checked by the queue controller logic to ensure no isochronous 
*        packets suffer more than this delay w.r.t to their eligibility time slot.
*        This represents the number of Queue Time Slots. The interval for the QTS 
*        can be configured using the register in Qav Global Configuration, Offset 0x2.
*
* INPUTS:
*        limit - per-port isochronous delay limit.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavSetGlobalIsoDelayLmt
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U8        limit
);

/*******************************************************************************
* gqavGetGlobalIsoDelayLmt
*
* DESCRIPTION:
*        This routine get Isochronous queue delay Limit
*        This field represents a per-port isochronous delay limit that 
*        will be checked by the queue controller logic to ensure no isochronous 
*        packets suffer more than this delay w.r.t to their eligibility time slot.
*        This represents the number of Queue Time Slots. The interval for the QTS 
*        can be configured using the register in Qav Global Configuration, Offset 0x2.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        limit - per-port isochronous delay limit.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavGetGlobalIsoDelayLmt
(
    IN  GT_QD_DEV     *dev,
    OUT GT_U8        *limit
);

/*******************************************************************************
* gqavSetGlobalIsoMonEn
*
* DESCRIPTION:
*       This routine set Isochronous monitor enable
*        Set GT_TRUE: this bit enables the statistics gathering capabilities stated
*        in PTP Global Status Registers Offset 0xD, 0xE and 0xF. Once enabled, the
*        software is expected to program the IsoMonPort (PTP Global Status Offset
*        0xD) indicating which port of the device does the software wants to monitor.
*        Upon setting this bit, the hardware collects IsoHiDisCtr, IsoLoDisCtr and
*        IsoSchMissCtr values for the port indicated by IsoMonPort till this bit is 
*        set to a zero.
*        Set GT_FALSE: this bit disables the statistics gathering capabilities.
*
* INPUTS:
*        en - GT_TRUE / GT_FALSE.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavSetGlobalIsoMonEn
(
    IN  GT_QD_DEV     *dev,
    IN  GT_BOOL        en
);

/*******************************************************************************
* gqavGetGlobalIsoMonEn
*
* DESCRIPTION:
*        This routine get Isochronous monitor enable
*        Set GT_TRUE: this bit enables the statistics gathering capabilities stated
*        in PTP Global Status Registers Offset 0xD, 0xE and 0xF. Once enabled, the
*        software is expected to program the IsoMonPort (PTP Global Status Offset
*        0xD) indicating which port of the device does the software wants to monitor.
*        Upon setting this bit, the hardware collects IsoHiDisCtr, IsoLoDisCtr and
*        IsoSchMissCtr values for the port indicated by IsoMonPort till this bit is 
*        set to a zero.
*        Set GT_FALSE: this bit disables the statistics gathering capabilities.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        en - GT_TRUE / GT_FALSE.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavGetGlobalIsoMonEn
(
    IN  GT_QD_DEV     *dev,
    OUT GT_BOOL        *en
);

/*******************************************************************************
* gqavSetGlobalIsoMonPort
*
* DESCRIPTION:
*        This routine set Isochronous monitoring port.
*        This field is updated by software along with Iso Mon En bit 
*        (Qav Global Status, offset 0xD) and it indicates the port number that 
*        the software wants the hardware to start monitoring i.e., start updating 
*        IsoHiDisCtr, IsoLoDisCtr and IsoSchMissCtr. The queue controller clears 
*        the above stats when IsoMonPort is changed..
*
* INPUTS:
*        port -  port number .
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavSetGlobalIsoMonPort
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U8         port
);

/*******************************************************************************
* gqavGetGlobalIsoMonPort
*
* DESCRIPTION:
*        This routine get Isochronous monitoring port.
*        This field is updated by software along with Iso Mon En bit 
*        (Qav Global Status, offset 0xD) and it indicates the port number that 
*        the software wants the hardware to start monitoring i.e., start updating 
*        IsoHiDisCtr, IsoLoDisCtr and IsoSchMissCtr. The queue controller clears 
*        the above stats when IsoMonPort is changed..
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        port -  port number.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavGetGlobalIsoMonPort
(
    IN  GT_QD_DEV     *dev,
    OUT GT_U8         *port
);

/*******************************************************************************
* gqavSetGlobalIsoHiDisCtr
*
* DESCRIPTION:
*        This routine set Isochronous hi queue discard counter.
*        This field is updated by hardware when instructed to do so by 
*        enabling the IsoMonEn bit in Qav Global Status Register Offset 0xD. 
*        This is an upcounter of number of isochronous hi packets discarded 
*        by Queue Controller.
*
* INPUTS:
*        disCtr - upcounter of number of isochronous hi packets discarded 
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavSetGlobalIsoHiDisCtr
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U8        disCtr
);

/*******************************************************************************
* gqavGetGlobalIsoHiDisCtr
*
* DESCRIPTION:
*        This routine get Isochronous hi queue discard counter.
*        This field is updated by hardware when instructed to do so by 
*        enabling the IsoMonEn bit in Qav Global Status Register Offset 0xD. 
*        This is an upcounter of number of isochronous hi packets discarded 
*        by Queue Controller.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        disCtr - upcounter of number of isochronous hi packets discarded 
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavGetGlobalIsoHiDisCtr
(
    IN  GT_QD_DEV     *dev,
    OUT GT_U8        *disCtr
);

/*******************************************************************************
* gqavSetGlobalIsoLoDisCtr
*
* DESCRIPTION:
*        This routine set Isochronous Lo queue discard counter.
*        This field is updated by hardware when instructed to do so by 
*        enabling the IsoMonEn bit in Qav Global Status Register Offset 0xD. 
*        This is an upcounter of number of isochronous lo packets discarded 
*        by Queue Controller.
*
* INPUTS:
*        disCtr - upcounter of number of isochronous lo packets discarded 
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavSetGlobalIsoLoDisCtr
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U8        disCtr
);

/*******************************************************************************
* gqavGetGlobalIsoLoDisCtr
*
* DESCRIPTION:
*        This routine set Isochronous Lo queue discard counter.
*        This field is updated by hardware when instructed to do so by 
*        enabling the IsoMonEn bit in Qav Global Status Register Offset 0xD. 
*        This is an upcounter of number of isochronous lo packets discarded 
*        by Queue Controller.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        disCtr - upcounter of number of isochronous lo packets discarded 
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqavGetGlobalIsoLoDisCtr
(
    IN  GT_QD_DEV     *dev,
    OUT GT_U8        *disCtr
);
/*******************************************************************************
* gqavGetReg
*
* DESCRIPTION:
*       This routine reads QAV register.
*
* INPUTS:
*       port         - logical port number.
*       regOffset    - register to read
*
* OUTPUTS:
*        data        - register data
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gqavGetReg
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_U32        regOffset,
    OUT GT_U32        *data
);

/*******************************************************************************
* gqavSetReg
*
* DESCRIPTION:
*       This routine writes data to QAV register.
*
* INPUTS:
*       port         - logical port number
*       regOffset    - register to be written
*        data        - data to be written
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gqavSetReg
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_U32        regOffset,
    IN  GT_U32        data
);

/*******************************************************************************
* gqbvGetReg
*
* DESCRIPTION:
*       This routine reads QBV register.
*
* INPUTS:
*       port         - logical port number.
*       regOffset    - register to read
*
* OUTPUTS:
*        data        - register data
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gqbvGetReg
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_U32        regOffset,
    OUT GT_U32        *data
);

/*******************************************************************************
* gqbvSetReg
*
* DESCRIPTION:
*       This routine writes data to QBV register.
*
* INPUTS:
*       port         - logical port number
*       regOffset    - register to be written
*        data        - data to be written
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gqbvSetReg
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_U32        regOffset,
    IN  GT_U32        data
);

/* gqbvPortPointDataSet  */
/*******************************************************************************
* gqbvPortPointDataSet
*
* DESCRIPTION:
*        Set indirect register pointer control data from QBV Global registers. 
*
* INPUTS:
*        port         - logical port number
*        point - The Pointer bits are used to access the Index registers as follows:
*                0x00 to 0x0F: Qbv Entries 
*                0x10 to 0x1F: Reserved for future use
*                0x20: Guard Band Entry 
*                0x21 to 0x7F: Reserved for future use
*        data -  QBV Global configuration data write to the indirect register 
*                pointed to by the point above.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqbvPortPointDataSet
(
    IN  GT_QD_DEV         *dev,
    IN  GT_LPORT    port,
    IN  GT_U8             point,
    IN  GT_U8             data
);

/*******************************************************************************
* gqbvPortPointDataGet
*
* DESCRIPTION:
*        Get indirect register pointer control data from QBV Global registers. 
*
* INPUTS:
*        port         - logical port number
*        point - The Pointer bits are used to access the Index registers as follows:
*                0x00 to 0x0F: Qbv Entries 
*                0x10 to 0x1F: Reserved for future use
*                0x20: Guard Band Entry 
*                0x21 to 0x7F: Reserved for future use
*
* OUTPUTS:
*        data -  QBV Global configuration data read from the indirect register 
*                pointed to by the point above.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqbvPortPointDataGet
(
    IN  GT_QD_DEV         *dev,
    IN  GT_LPORT    port,
    IN  GT_U8             point,
    OUT  GT_U8            *data
);

/* gqbvSetPortDelay  */
/*******************************************************************************
* gqbvSetPortDelay
*
* DESCRIPTION:
*        Set port delay to QBV registers. 
*
* INPUTS:
*        port         - logical port number
*        delay -  QBV Port Delay Time..
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqbvSetPortDelay
(
    IN  GT_QD_DEV         *dev,
    IN  GT_LPORT    port,
    IN  GT_U16             delay
);

/* gqbvGetPortDelay  */
/*******************************************************************************
* gqbvGetPortDelay
*
* DESCRIPTION:
*        Get port delay from QBV registers. 
*
* INPUTS:
*        port         - logical port number
*
* OUTPUTS:
*        delay -  QBV Port Delay Time..
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqbvGetPortDelay
(
    IN  GT_QD_DEV         *dev,
    IN  GT_LPORT    port,
    OUT  GT_U16           *delay
);

/* gqbvSetPortWindowTime  */
/*******************************************************************************
* gqbvSetPortWindowTime
*
* DESCRIPTION:
*        Set window time or guard band bytes to QBV registers. 
*
* INPUTS:
*        port         - logical port number
*        windowTime -  QBV Port window Time or band bytes.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqbvSetPortWindowTime
(
    IN  GT_QD_DEV         *dev,
    IN  GT_LPORT    port,
    IN  GT_U16             windowTime
);

/* gqbvGetPortWindowTime  */
/*******************************************************************************
* gqbvGetPortWindowTime
*
* DESCRIPTION:
*        Get window time or guard band bytes to QBV registers. 
*
* INPUTS:
*        port         - logical port number
*
* OUTPUTS:
*        windowTime -  QBV Port window Time or band bytes.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqbvGetPortWindowTime
(
    IN  GT_QD_DEV         *dev,
    IN  GT_LPORT    port,
    OUT  GT_U16           *windowTime
);

/* gqbvSetPortGuardBand  */
/*******************************************************************************
* gqbvSetPortGuardBand
*
* DESCRIPTION:
*        Set  guard band bytes to QBV registers. 
*
* INPUTS:
*        port         - logical port number
*        guardBand -  QBV Port guard band bytes.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqbvSetPortGuardBand
(
    IN  GT_QD_DEV         *dev,
    IN  GT_LPORT    port,
    IN  GT_U16             guardBand
);

/* gqbvGetPortGuardBand  */
/*******************************************************************************
* gqbvGetPortGuardBand
*
* DESCRIPTION:
*        Get  guard band bytes to QBV registers. 
*
* INPUTS:
*        port         - logical port number
*
* OUTPUTS:
*        guardBand -  QBV Port guard band bytes.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gqbvGetPortGuardBand
(
    IN  GT_QD_DEV         *dev,
    IN  GT_LPORT    port,
    OUT  GT_U16           *guardBand
);

/*******************************************************************************
* gsysSetUseIpMappingTable
*
* DESCRIPTION:
*        This API set to use IP Frame Priorities from this table. 
*        Set GT_TRUE:  The IP_FPRI data in this table is used as the frame�s 
*            initial IP_FPRI.
*        Set GT_FALSE: The IP_FPRI data in this table is ignored. Instead the 
*            frame�s initial IP_FPRI is generated by using the frame�s IP_QPRI
*            as the IP_FPRI�s upper two bits, and the IP_FPRI�s lowest bit comes 
*            from bit 0 of the frame�s source port�s Default PRI (Port offset 0x07).
*
* INPUTS:
*        en    - [GT_TRUE] / [GT_FALSE]
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysSetUseIpMappingTable
(
    IN  GT_QD_DEV         *dev,
    IN  GT_BOOL            en
);

/*******************************************************************************
* gsysGetUseIpMappingTable
*
* DESCRIPTION:
*        This API get to use IP Frame Priorities from this table. 
*        Set GT_TRUE:  The IP_FPRI data in this table is used as the frame�s 
*            initial IP_FPRI.
*        Set GT_FALSE: The IP_FPRI data in this table is ignored. Instead the 
*            frame�s initial IP_FPRI is generated by using the frame�s IP_QPRI
*            as the IP_FPRI�s upper two bits, and the IP_FPRI�s lowest bit comes 
*            from bit 0 of the frame�s source port�s Default PRI (Port offset 0x07).
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        en    - [GT_TRUE] / [GT_FALSE]
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGetUseIpMappingTable
(
    IN  GT_QD_DEV         *dev,
    IN  GT_BOOL            *en
);

/*******************************************************************************
* gsysSetIpMappingPrio
*
* DESCRIPTION:
*        Set IPv4 and IPv6 Frame Priority Mapping, and 
*        IPv4 and IPv6 Queue Priority Mapping.
*       The ipFpri value is used as the frames initial FPRI when the frame is 
*        an IPv4 or an IPv6 frame, and the port�s InitialPri (Port offset 0x04) 
*        is configured to use IP FPri�s.
*       The ipQpri value is used as the frame�s initial QPRI when the frame is 
*        an IPv4 or an IPv6 frame, and the port�s InitialPri and TagIfBoth 
*        registers (Port offset 0x04) are configured to use IP QPri�s.
*
* INPUTS:
*        point - Pointer to the Ip Mapping Table.
*                0 - 0x3f;
*        ipFpri -  The value is 0 - 7
*        ipQpri -  The value is 0 - 3.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysSetIpMappingPrio
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U32            point,
    IN  GT_U8            ipFpri,
    IN  GT_U8            ipQpri
);

/*******************************************************************************
* gsysGetIpMappingPrio
*
* DESCRIPTION:
*        Get IPv4 and IPv6 Frame Priority Mapping, and 
*        IPv4 and IPv6 Queue Priority Mapping.
*       The ipFpri value is used as the frames initial FPRI when the frame is 
*        an IPv4 or an IPv6 frame, and the port�s InitialPri (Port offset 0x04) 
*        is configured to use IP FPri�s.
*        The ipQpri value is used as the frame�s initial QPRI when the frame is 
*        an IPv4 or an IPv6 frame, and the port�s InitialPri and TagIfBoth 
*        registers (Port offset 0x04) are configured to use IP QPri�s.
*
* INPUTS:
*        point - Pointer to the Ip Mapping Table.
*                0 - 0x3f;
*
* OUTPUTS:
*        ipFpri -  The value is 0 - 7
*        ipQpri -  The value is 0 - 3.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS gsysGetIpMappingPrio
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U32            point,
    OUT  GT_U8            *ipFpri,
    OUT  GT_U8            *ipQpri
);

/*******************************************************************************
* gsysReadEeprom
*
* DESCRIPTION:
*        Read EEPROM from EEPROM�s address where the EEOp is performed.
*
* INPUTS:
*        addr - EEPROM Address. 
*
* OUTPUTS:
*        data -  Data that was read back from the EEPROM.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysReadEeprom
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U16            addr,
    OUT  GT_U16            *data
);

/*******************************************************************************
* gsysWriteEeprom
*
* DESCRIPTION:
*        Write EEPROM at the EEPROM�s address where the EEOp is performed.
*
* INPUTS:
*        addr - EEPROM Address. 
*        data - Data to be written to the EEPROM
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysWriteEeprom
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U16            addr,
    IN  GT_U16            data
);

/*******************************************************************************
* gsysRestartEeprom
*
* DESCRIPTION:
*        Restart Register Loader execution at the EEPROM�s address where the EEOp 
*        is performed
*
* INPUTS:
*        addr - EEPROM Address. .
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysRestartEeprom
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8            addr
);

/*******************************************************************************
* gsysHaltEeprom
*
* DESCRIPTION:
*        Halt (stop executing the EEPROM if its not already stopped)
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysHaltEeprom
(
    IN  GT_QD_DEV         *dev
);

/*******************************************************************************
* gsysGetStEeprom
*
* DESCRIPTION:
*        Get EEPROM status. They are Register Loader Running status and EEPROM 
*        Write Enable status
*        runSt is GT_TRUE: Register Loader Running, whenever the register loader 
*            is busy executing the instructions contained in the EEPROM.
*        writeEn is GT_TRUE: EEPROM Write Enable, that indicates that writing to 
*            the EEPROM is possible. 
*        writeEn is GT_FALSE: the Write EEPROM EEOp above will not do anything.
*            This reflects the value of the EE_WE configuration pin after Reset.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        runSt   -   [GT_TRUE] / [GT_FALSE)
*        writeEn -   [GT_TRUE] / [GT_FALSE)
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGetStEeprom
(
    IN  GT_QD_DEV         *dev,
    OUT GT_BOOL            *writeEn,
    OUT GT_BOOL            *runSt
);

/*******************************************************************************
* gsysSetScratchMiscCtrl
*
* DESCRIPTION:
*        Set Scratch and Misc control data to the Scratch and Misc Control register. 
*        The registers of Scratch and Misc control are.
*                Scratch Byte 0
*                Scratch Byte 1
*                GPIO Configuration
*                Reserved for future use
*                GPIO Direction
*                GPIO Data
*                CONFIG Data 0
*                CONFIG Data 1
*                CONFIG Data 2
*                CONFIG Data 3
*                SyncE & TAICLK125�s Drive
*                P5�s & CLK125�s Clock Drive
*                P6�s Clock Drive
*                EEPROM Pad drive
*
* INPUTS:
*        point - Pointer to the Scratch and Misc. Control register.
*        data  - Scratch and Misc. Control data written to the register 
*                pointed to by the point above.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysSetScratchMiscCtrl
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8             point,
    IN  GT_U8            data
);

/*******************************************************************************
* gsysGetScratchMiscCtrl
*
* DESCRIPTION:
*        Get Scratch and Misc control data from the Scratch and Misc Control register. 
*        The register of Scratch and Misc control are.
*                Scratch Byte 0
*                Scratch Byte 1
*                GPIO Configuration
*                Reserved for future use
*                GPIO Direction
*                GPIO Data
*                CONFIG Data 0
*                CONFIG Data 1
*                CONFIG Data 2
*                CONFIG Data 3
*                SyncE & TAICLK125�s Drive
*                P5�s & CLK125�s Clock Drive
*                P6�s Clock Drive
*                EEPROM Pad drive

*
* INPUTS:
*        point - Pointer to the Scratch and Misc. Control register.
*
* OUTPUTS:
*        data - Scratch and Misc. Control data read from the register 
*                pointed to by the point above.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGetScratchMiscCtrl
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8             point,
    OUT  GT_U8            *data
);

/*******************************************************************************
* gsysSetScratchBits
*
* DESCRIPTION:
*        Set bits to the Scratch and Misc Control register <scratch byte 0 and 1>.
*        These bits are 100% available to software for whatever purpose desired. 
*        These bits do not connect to any hardware function.
*
* INPUTS:
*        scritch - written bits.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysSetScratchBits
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U16            scratch
);

/*******************************************************************************
* gsysGetScratchBits
*
* DESCRIPTION:
*        Get bits from the Scratch and Misc Control register <scratch byte 0 and 1>.
*        These bits are 100% available to software for whatever purpose desired. 
*        These bits do not connect to any hardware function.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        scritch - read bits.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGetScratchBits
(
    IN  GT_QD_DEV         *dev,
    OUT  GT_U16            *scratch
);

/*******************************************************************************
* gsysSetGpioConfigMod
*
* DESCRIPTION:
*        Set bits to the Scratch and Misc Control register <GPIO Configuration>
*        to configure GPIO mode.
*        The bits are shared General Purpose Input Output mode Bits:
*        Bit 15 - GT_GPIO_BIT_14:    1:GPIO[14]    0:P5_OUTCLK
*          ...
*        Bit 6 - GT_GPIO_BIT_6:    1:GPIO[6]    0:SE_RCLK1
*        Bit 5 - GT_GPIO_BIT_5:    1:GPIO[5]    0:SE_RCLK0
*        Bit 4 - GT_GPIO_BIT_4:    1:GPIO[4]    0:
*        Bit 3 - GT_GPIO_BIT_3:    1:GPIO[3]    0:
*        Bit 2 - GT_GPIO_BIT_2:    1:GPIO[2]    0:
*        Bit 1 - GT_GPIO_BIT_1:    1:GPIO[1]    0:P6_COL
*        Bit 0 - GT_GPIO_BIT_0:    1:GPIO[0]    0:P6_CRS
*        Now,  bits are read only, except for 0, and 7. 
*
* INPUTS:
*        mode - OR [GT_GPIO_BIT_x]
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysSetGpioConfigMod
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U32            mode
);

/*******************************************************************************
* gsysGetGpioConfigMod
*
* DESCRIPTION:
*        Get mode from the Scratch and Misc Control register <GPIO Configuration>.
*        The bits are shared General Purpose Input Output mode Bits:
*        Bit 15 - GT_GPIO_BIT_14:    1:GPIO[14]    0:P5_OUTCLK
*          ...
*        Bit 6 - GT_GPIO_BIT_6:    1:GPIO[6]    0:SE_RCLK1
*        Bit 5 - GT_GPIO_BIT_5:    1:GPIO[5]    0:SE_RCLK0
*        Bit 4 - GT_GPIO_BIT_4:    1:GPIO[4]    0:
*        Bit 3 - GT_GPIO_BIT_3:    1:GPIO[3]    0:
*        Bit 2 - GT_GPIO_BIT_2:    1:GPIO[2]    0:
*        Bit 1 - GT_GPIO_BIT_1:    1:GPIO[1]    0:P6_COL
*        Bit 0 - GT_GPIO_BIT_0:    1:GPIO[0]    0:P6_CRS
*        Now,  bits are read only, except for 0, and 7. 
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        mode - OR [GT_GPIO_BIT_x]
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGetGpioConfigMod
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U32            *mode
);

/*******************************************************************************
* gsysSetGpioPinControl
*
* DESCRIPTION:
*        Set GPIO pin control to the Scratch and Misc Control register <GPIO pin control>
*        General Purpose Input Output Interface pin x Control. This control is 
*        used to control alternate functions of the GPIO[x] pin when it is not 
*        being used as part of some other interface. This control has an effect 
*        only if the pin is enabled to be a GPIO pin, i.e., the GPIO[x] Mode bit
*        is a one (Register Index 0x60 of Scratch and Misc., Control). 
*        The options are as follows:
*            GT_GPIO_PIN_GPIO        
*            GT_GPIO_PIN_PTP_TRIG    
*            GT_GPIO_PIN_PTP_EVREQ   
*            GT_GPIO_PIN_PTP_EXTCLK  
*            GT_GPIO_PIN_RX_CLK0     
*            GT_GPIO_PIN_RX_CLK1     
*            GT_GPIO_PIN_SDET        
*            GT_GPIO_PIN_CLK125    
*  
* INPUTS:
*        pinNum  - pin number.
*        pinCtrl - pin control.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysSetGpioPinControl
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8            pinNum,
    IN  GT_U8            pinCtrl
);

/*******************************************************************************
* gsysGetGpioPinControl
*
* DESCRIPTION:
*        Get GPIO pin control to the Scratch and Misc Control register <GPIO pin control>.
*        General Purpose Input Output Interface pin x Control. This control is 
*        used to control alternate functions of the GPIO[x] pin when it is not 
*        being used as part of some other interface. This control has an effect 
*        only if the pin is enabled to be a GPIO pin, i.e., the GPIO[x] Mode bit
*        is a one (Register Index 0x60 of Scratch and Misc., Control). 
*        The options are as follows:
*            GT_GPIO_PIN_GPIO        
*            GT_GPIO_PIN_PTP_TRIG    
*            GT_GPIO_PIN_PTP_EVREQ   
*            GT_GPIO_PIN_PTP_EXTCLK  
*            GT_GPIO_PIN_RX_CLK0     
*            GT_GPIO_PIN_RX_CLK1     
*            GT_GPIO_PIN_SDET        
*            GT_GPIO_PIN_CLK125    
*  
* INPUTS:
*        pinNum  - pin number.
*
* OUTPUTS:
*        pinCtrl - pin control.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGetGpioPinControl
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8            pinNum,
    OUT GT_U8            *pinCtrl
);

/*******************************************************************************
* gsysSetGpioDirection
*
* DESCRIPTION:
*        Set Gpio direction to the Scratch and Misc Control register <GPIO Direction>.
*        The bits are used to control the direction of GPIO[6:0] or GPIO{14:0]. 
*        When a GPIO�s bit is set to a one that GPIO will become an input. When a
*        GPIO�s bit is cleared to a zero that GPIO will become an output
*        General Purpose Input Output direction bits are:
*        Bit 15 - GT_GPIO_BIT_14
*          ...
*        Bit 6 - GT_GPIO_BIT_6
*          ...
*        Bit 1 - GT_GPIO_BIT_1
*        Bit 0 - GT_GPIO_BIT_0
*
* INPUTS:
*        dir - OR [GT_GPIO_BIT_x]
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysSetGpioDirection
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U32            dir
);

/*******************************************************************************
* gsysGetGpioDirection
*
* DESCRIPTION:
*        Get Gpio direction from the Scratch and Misc Control register <GPIO Direction>.
*        The bits are used to control the direction of GPIO[6:0] or GPIO{14:0]. 
*        When a GPIO�s bit is set to a one that GPIO will become an input. When a
*        GPIO�s bit is cleared to a zero that GPIO will become an output
*        General Purpose Input Output direction bits are:
*        Bit 15 - GT_GPIO_BIT_14
*          ...
*        Bit 6 - GT_GPIO_BIT_6
*          ...
*        Bit 1 - GT_GPIO_BIT_1
*        Bit 0 - GT_GPIO_BIT_0
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        dir - OR [GT_GPIO_BIT_x]
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGetGpioDirection
(
    IN  GT_QD_DEV         *dev,
    OUT  GT_U32            *dir
);

/*******************************************************************************
* gsysSetGpioSMI
*
* DESCRIPTION:
*        Set Normal SMI to the Scratch and Misc Control register <GPIO Direction>.
*        The bit is used to control the  Normal SMI vs. GPIO mode. 
*        When P5_MODE is not equal to 0x1 or 0x2 the P5_COL and P5_CRS pins are 
*        not needed. In this case, when this bit is set to a one, the P5_COL and 
*        P5_CRS pins become MDIO_PHY and MDC_PHY, respectively, if the NO_CPU 
*        configuration pin was a one during reset. Else the pins become GPIO pins 7 and 8.
*
* INPUTS:
*        smi - SMI OR GT_GPIO
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysSetGpioSMI
(
    IN  GT_QD_DEV         *dev,
    IN  GT_BOOL           smi
);

/*******************************************************************************
* gsysGetGpioSMI
*
* DESCRIPTION:
*        Get Normal SMI from the Scratch and Misc Control register <GPIO Direction>.
*        The bit is used to control the  Normal SMI vs. GPIO mode. 
*        When P5_MODE is not equal to 0x1 or 0x2 the P5_COL and P5_CRS pins are 
*        not needed. In this case, when this bit is set to a one, the P5_COL and 
*        P5_CRS pins become MDIO_PHY and MDC_PHY, respectively, if the NO_CPU 
*        configuration pin was a one during reset. Else the pins become GPIO pins 7 and 8.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        smi - SMI OR GT_GPIO
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGetGpioSMI
(
    IN  GT_QD_DEV         *dev,
    OUT GT_BOOL           *smi
);

/*******************************************************************************
* gsysSetGpioData
*
* DESCRIPTION:
*        Set Gpio data to the Scratch and Misc Control register <GPIO data>.
*        When a GPIO�s bit is set to be an input, data written to this bit will go 
*        to a holding register but will not appear on the pin nor in this register. 
*        Reads of this register will return the actual, real-time, data that is 
*        appearing on the GPIO�s pin.
*        When a GPIO�s bit is set to be an output, data written to this bit will go 
*        to a holding register and will appear on the GPIO�s pin. Reads of this register 
*        will return the actual, real-time, data that is appearing on the GPIO�s pin 
*        (which in this case should be the data written, but if its isn�t that would 
*        be an indication of a conflict).
*        When a pin�s direction changes from input to output, the data last written 
*        to the holding register appears on the GPIO�s pin
*        General Purpose Input Output data bits are:
*        Bit 15 - GT_GPIO_BIT_14
*          ...
*        Bit 6 - GT_GPIO_BIT_6
*          ...
*        Bit 5 - GT_GPIO_BIT_5
*        Bit 4 - GT_GPIO_BIT_4
*        Bit 3 - GT_GPIO_BIT_3
*        Bit 2 - GT_GPIO_BIT_2
*        Bit 1 - GT_GPIO_BIT_1
*        Bit 0 - GT_GPIO_BIT_0
*
* INPUTS:
*        data - OR [GT_GPIO_BIT_x]
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysSetGpioData
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U32            data
);

/*******************************************************************************
* gsysGetGpioData
*
* DESCRIPTION:
*        get Gpio data to the Scratch and Misc Control register <GPIO data>.
*        When a GPIO�s bit is set to be an input, data written to this bit will go 
*        to a holding register but will not appear on the pin nor in this register. 
*        Reads of this register will return the actual, real-time, data that is 
*        appearing on the GPIO�s pin.
*        When a GPIO�s bit is set to be an output, data written to this bit will go 
*        to a holding register and will appear on the GPIO�s pin. Reads of this register 
*        will return the actual, real-time, data that is appearing on the GPIO�s pin 
*        (which in this case should be the data written, but if its isn�t that would 
*        be an indication of a conflict).
*        When a pin�s direction changes from input to output, the data last written 
*        to the holding register appears on the GPIO�s pin
*        General Purpose Input Output data bits are:
*        Bit 15 - GT_GPIO_BIT_14
*          ...
*        Bit 6 - GT_GPIO_BIT_6
*          ...
*        Bit 5 - GT_GPIO_BIT_5
*        Bit 4 - GT_GPIO_BIT_4
*        Bit 3 - GT_GPIO_BIT_3
*        Bit 2 - GT_GPIO_BIT_2
*        Bit 1 - GT_GPIO_BIT_1
*        Bit 0 - GT_GPIO_BIT_0
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        data - OR [GT_GPIO_BIT_x]
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGetGpioData
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U32            *data
);

/*******************************************************************************
* gsysGetConfigData
*
* DESCRIPTION:
*        Get Reset Configuration Pin Data 0-3. 
*        These registers return the values observed after a hardware Reset on the 
*        listed CONFIG data listed below.
*            Config data 0:
*              Bit    Config    Pin's Primary Name
*                0    USER[0]        P6_OUTD[5]
*                1    USER[1]        P6_OUTD[6]
*                2    USER[2]        P6_OUTD[7]
*                3    ADDR[0]        P5_OUTD[0]
*                4    ADDR[1]        P5_OUTD[5]
*                5    ADDR[2]        P5_OUTD[6]
*                6    ADDR]3]        P5_OUTD[7]
*                7    ADDR[4]        P5_OUTD[1]
*            Config data 1:
*                0    LED_SEL[0]    P1_LED
*                1    LED_SEL[1]    P2_LED
*                2    4COL P3_LED
*                3    NormCx        P4_LED
*                4    Jumbo        P0_LED
*                5    EE_WE        EE_CS/C2_LED
*                6    FD_FLOW        EE_CLK/C1_LED
*                7    HD_FLOW        EE_DIN/C0_LED
*            Config data 2:
*                0    P5_MODE[0]    P5_OUTD[2]
*                1    P5_MODE[1]    P5_OUTD[3]
*                2    P5_MODE[2]    P5_OUTD[4]
*                3    Reserved for future use
*                4    P6_MODE[0]    P6_OUTD[2]
*                5    P6_MODE[1]    P6_OUTD[3]
*                6    P6_MODE[2]    P6_OUTD[4]
*                7    Reserved for future use
*            Config data 3:
*                0    RMU_MODE[0] P6_OUTD[0]
*                1    RMU_MODE[1] P6_OUTD[1]
*                2    S_VDDOS[0]    PTP_TRIG 
*                3    CLK125EN    CLK125
*                4    P5_VDDOS[0] P5_GTXCLK
*                5    P5_VDDOS[1] P5_OUTEN
*                6    P6_VDDOS[0] P5_GTXCLK
*                7    P6_VDDOS[1] P6_OUTEN
* INPUTS:
*        None.
*
* OUTPUTS:
*       cfgDat - GT_CONFIG_DTTA
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGetConfigData
(
    IN  GT_QD_DEV             *dev,
    OUT  GT_CONFIG_DATA        *cfgData
);

/*******************************************************************************
* gsysSetSyncETai
*
* DESCRIPTION:
*        Set SyncE and Tai to the Scratch and Misc. Control register <SyncE and TAI pad>.
*
* INPUTS:
*        zpr - ZPR for SyncE and TAI
*        znr - ZNR for SyncE and TAI
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysSetSyncETai
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8            zpr,
    IN  GT_U8            znr
);

/*******************************************************************************
* gsysSetGpioxPortStallVect
*
* DESCRIPTION:
*     Set GPIO X�s Port Stall Vector. The value in this register is sent to the transmit
*     portion of all the ports in the device when GPIO X�s Port Stall En (below) is set
*     to a one and the value on the device�s GPIO pin matches the value in GPIO
*     X�s Port Stall Value (below) and the GPIO pin is a GPIO input. The assertion
*     of this vector to the ports persists as long as the function is enabled (i.e., GPIO
*     X Port Stall En equals a one) and as long as the value on the pin matches the
*     programmed stall value (GPIO X Port Stall Value) and as long as the pin
*     remains a GPIO input.
*     When a port�s bit is set to a one in this vector, that port or ports will stop
*     tranmitting their next frame (the current frame is not affected) until this vector
*     is deasserted. Port 0 is controlled by bit 0, port 1 by bit 1, etc.
*
* INPUTS:
*        gpioNum - GPIO number.
*        portStallVec - GPIO Num�s Port Stall Vector.
*        portStallEn - GPIO Num�s Port Stall Enable.
*        portStallValue - GPIO Num�s Port Stall Value.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysSetGpioxPortStallVect
(
    IN  GT_QD_DEV        *dev,
    IN  GT_U8            gpioNum,
    IN  GT_U8            portStallVec,
    IN  GT_U8            portStallEn,
    IN  GT_U8            portStallValue
);

/*******************************************************************************
* gsysGetGpioxPortStallVect
*
* DESCRIPTION:
*     Get GPIO X�s Port Stall Vector. The value in this register is sent to the transmit
*     portion of all the ports in the device when GPIO X�s Port Stall En (below) is set
*     to a one and the value on the device�s GPIO pin matches the value in GPIO
*     X�s Port Stall Value (below) and the GPIO pin is a GPIO input. The assertion
*     of this vector to the ports persists as long as the function is enabled (i.e., GPIO
*     X Port Stall En equals a one) and as long as the value on the pin matches the
*     programmed stall value (GPIO X Port Stall Value) and as long as the pin
*     remains a GPIO input.
*     When a port�s bit is set to a one in this vector, that port or ports will stop
*     tranmitting their next frame (the current frame is not affected) until this vector
*     is deasserted. Port 0 is controlled by bit 0, port 1 by bit 1, etc.
*
* INPUTS:
*        gpioNum - GPIO number.
*
* OUTPUTS:
*        portStallVec - GPIO Num�s Port Stall Vector.
*        portStallEn - GPIO Num�s Port Stall Enable.
*        portStallValue - GPIO Num�s Port Stall Value.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGetGpioxPortStallVect
(
    IN  GT_QD_DEV        *dev,
    IN  GT_U8            gpioNum,
    OUT  GT_U8           *portStallVec,
    OUT  GT_U8           *portStallEn,
    OUT  GT_U8           *portStallValue
);

/*******************************************************************************
* gsysClearxEEERxStatusCounter
*
* DESCRIPTION:
*     Clear Receive Low Power Idle Count. The counter tracks number of low
*     power idle command that Port 0 received. This counter will wrap.
*
* INPUTS:
*        port - port number.
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysClearxEEERxStatusCounter
(
    IN  GT_QD_DEV        *dev,
    IN  GT_U8            port,
    OUT  GT_U8           *eeeStCount
);

/*******************************************************************************
* gsysGetxEEERxStatusCounter
*
* DESCRIPTION:
*     Get Receive Low Power Idle Count. The counter tracks number of low
*     power idle command that Port 0 received. This counter will wrap.
*
* INPUTS:
*        port - port number.
*
* OUTPUTS:
*        eeeStCount - EEE receive status counter.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGetxEEERxStatusCounter
(
    IN  GT_QD_DEV        *dev,
    IN  GT_U8            port,
    OUT  GT_U8           *eeeStCount
);

/*******************************************************************************
* gsysGetxEEETrStatusCounter
*
* DESCRIPTION:
*     Get Transmit Low Power Idle Count. The counter tracks number of low
*     power idle command that Port 0 initiated. This counter will wrap.
*
* INPUTS:
*        port - port number.
*
* OUTPUTS:
*        eeeStCount - EEE Transmit status counter.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGetxEEETrStatusCounter
(
    IN  GT_QD_DEV        *dev,
    IN  GT_U8            port,
    OUT  GT_U8           *eeeStCount
);

/*******************************************************************************
* gsysClearxEEETrStatusCounter
*
* DESCRIPTION:
*     Clear Transmit Low Power Idle Count. The counter tracks number of low
*     power idle command that Port 0 initiated. This counter will wrap.
*
* INPUTS:
*        port - port number.
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysClearxEEETrStatusCounter
(
    IN  GT_QD_DEV        *dev,
    IN  GT_U8            port
);

/*******************************************************************************
* gsysGetxEEERxStates
*
* DESCRIPTION:
*     Get all Ports Receive Low Power Idle State. This bit indicates the receiving
*     port is in the state of low power idle mode.
*
* INPUTS:
*        port - port number.
*
* OUTPUTS:
*        eeeStR - EEE receive state.
*        eeeStT - EEE Transmitr state.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGetxEEERxStates
(
    IN  GT_QD_DEV        *dev,
    IN  GT_U8            port,
    OUT  GT_U8           *eeeStR,
    OUT  GT_U8           *eeeStT
);

/*******************************************************************************
* gsysSetPowerCtrl
*
* DESCRIPTION:
*        Set Power Management Control. These bits are used to control the power
*        management of the switch core (this works in conjunction with the EEE
*        features of this device). When these bits are zero, core power
*        management is disabled (EEE can still be used in this mode to save PHY
*        power).
*        When these bits are non zero, core power management is enabled and
*        will take effect �n� core clocks or �n� uSec or �n� mSec (where �n� = the
*        value of this register and the selection of the rate is controlled by the
*        PwrRate bits in index 0x0B) after all of the following have occurred:
*        o All Tx Queues on all ports are empty.
*        o All MAC�s Tx and Rx paths are idle.
*        o The MDC/MDIO bus is idle and all pending write data has been  written.
*        o All blocks with I/O Busy bits (ATU, VTU, etc.) need to idle (i.e., not  busy).
*        The RMU is idle
*
* INPUTS:
*        pwCtrl - Power Management Control
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysSetPowerCtrl
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8           pwCtrl
);

/*******************************************************************************
* gsysGetPowerCtrl
*
* DESCRIPTION:
*        Get Power Management Control. These bits are used to control the power
*        management of the switch core (this works in conjunction with the EEE
*        features of this device). When these bits are zero, core power
*        management is disabled (EEE can still be used in this mode to save PHY
*        power).
*        When these bits are non zero, core power management is enabled and
*        will take effect �n� core clocks or �n� uSec or �n� mSec (where �n� = the
*        value of this register and the selection of the rate is controlled by the
*        PwrRate bits in index 0x0B) after all of the following have occurred:
*        o All Tx Queues on all ports are empty.
*        o All MAC�s Tx and Rx paths are idle.
*        o The MDC/MDIO bus is idle and all pending write data has been  written.
*        o All blocks with I/O Busy bits (ATU, VTU, etc.) need to idle (i.e., not  busy).
*        The RMU is idle
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        pwCtrl - Power Management Control
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGetPowerCtrl
(
    IN  GT_QD_DEV         *dev,
    OUT GT_U8           *pwCtrl
);

/*******************************************************************************
* gsysSetWakeTimeGE
*
* DESCRIPTION:
*        Set EEE Wake Time for Gigabit Ethernet ports. In order for PHY to exit low power
*        mode and return back to normal mode, a wake up period is needed. These
*        bits determine the time that switch must wait for the PHY to completely exit
*        low power mode before the switch can start to transmit a packet. The value in
*        this register is in micro seconds (uSec), mille seconds (mSec) or seconds as
*        determined by WakeRate in Index 0x0B above. The default value of 0x11 is
*        17uSec. This register supports the full range of 0us to 255us
*
* INPUTS:
*        wakeTime - EEE Wake Time
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysSetWakeTimeGE
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8           wakeTime
);

/*******************************************************************************
* gsysGetWakeTimeGE
*
* DESCRIPTION:
*        Get EEE Wake Time for Gigabit Ethernet ports. In order for PHY to exit low power
*        mode and return back to normal mode, a wake up period is needed. These
*        bits determine the time that switch must wait for the PHY to completely exit
*        low power mode before the switch can start to transmit a packet. The value in
*        this register is in micro seconds (uSec), mille seconds (mSec) or seconds as
*        determined by WakeRate in Index 0x0B above. The default value of 0x11 is
*        17uSec. This register supports the full range of 0us to 255us
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        wakeTime - EEE Wake Time
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGetWakeTimeGE
(
    IN  GT_QD_DEV         *dev,
    OUT GT_U8           *wakeTime
);

/*******************************************************************************
* gsysSetAsrtTime
*
* DESCRIPTION:
*        Set EEE Assertion Time. When the PHY is asked to enter low power mode it
*        cannot immediately leave this mode. This timer determines the time
*        requirement for the PHY to fully power down before a wake up request can be
*        processed. The value in this register is in micro seconds (uSec), mille
*        seconds (mSec) or seconds as determined by AsrtRate in Index 0x0B above.
*        An AsrtTime of 0x0 has the low power idle code deasserted immediately when
*        an idle request is deasserted.
*
* INPUTS:
*        asrtTime - EEE Assertion Time
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysSetAsrtTime
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8           asrtTime
);

/*******************************************************************************
* gsysGetAsrtTime
*
* DESCRIPTION:
*        Get EEE Assertion Time. When the PHY is asked to enter low power mode it
*        cannot immediately leave this mode. This timer determines the time
*        requirement for the PHY to fully power down before a wake up request can be
*        processed. The value in this register is in micro seconds (uSec), mille
*        seconds (mSec) or seconds as determined by AsrtRate in Index 0x0B above.
*        An AsrtTime of 0x0 has the low power idle code deasserted immediately when
*        an idle request is deasserted.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        asrtTime - EEE Assertion Time
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGetAsrtTime
(
    IN  GT_QD_DEV         *dev,
    OUT GT_U8           *asrtTime
);

/*******************************************************************************
* gsysSetWakeTimeFE
*
* DESCRIPTION:
*        Set EEE Wake Time for Fast Ethernet ports. In order for PHY to exit low power
*        mode and return back to normal mode, a wake up period is needed. These
*        bits determine the time that switch must wait for the PHY to completely exit
*        low power mode before the switch can start to transmit a packet. The value in
*        this register is in micro seconds (uSec), mille seconds (mSec) or seconds as
*        determined by WakeRate in Index 0x0B above. The default value of 0x11 is
*        17uSec. This register supports the full range of 0us to 255us
*
* INPUTS:
*        wakeTime - EEE Wake Time
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysSetWakeTimeFE
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8           wakeTime
);

/*******************************************************************************
* gsysGetWakeTimeFE
*
* DESCRIPTION:
*        Get EEE Wake Time for Fast Ethernet ports. In order for PHY to exit low power
*        mode and return back to normal mode, a wake up period is needed. These
*        bits determine the time that switch must wait for the PHY to completely exit
*        low power mode before the switch can start to transmit a packet. The value in
*        this register is in micro seconds (uSec), mille seconds (mSec) or seconds as
*        determined by WakeRate in Index 0x0B above. The default value of 0x11 is
*        17uSec. This register supports the full range of 0us to 255us
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        wakeTime - EEE Wake Time
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGetWakeTimeFE
(
    IN  GT_QD_DEV         *dev,
    OUT GT_U8           *wakeTime
);

/*******************************************************************************
* gsysSetTxIdleTime
*
* DESCRIPTION:
*        Set EEE Transmit Idle Time. These bits determine the time that each TX port
*        must remain idle before telling the PHY to entering its low power state. The
*        value in this register is in micro seconds (uSec), mille seconds (mSec) or
*        seconds as determined by TxIdleRate in Index 0x0B above. If the Transmit
*        Idle Time is set to 0x0 then automatic low power mode is entered as soon as
*        the port�s egress queue is empty and has nothing more to send. 0x0 is a
*        special test case that will cause EEE power saving mode to be entered
*        right away when port�s EEE mode is enabled.
*
* INPUTS:
*        idleTime - EEE Transmit Idle Time
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysSetTxIdleTime
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8           idleTime
);

/*******************************************************************************
* gsysGetTxIdleTime
*
* DESCRIPTION:
*        Get EEE Transmit Idle Time. These bits determine the time that each TX port
*        must remain idle before telling the PHY to entering its low power state. The
*        value in this register is in micro seconds (uSec), mille seconds (mSec) or
*        seconds as determined by TxIdleRate in Index 0x0B above. If the Transmit
*        Idle Time is set to 0x0 then automatic low power mode is entered as soon as
*        the port�s egress queue is empty and has nothing more to send. 0x0 is a
*        special test case that will cause EEE power saving mode to be entered
*        right away when port�s EEE mode is enabled.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        idleTime - EEE Transmit Idle Time
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGetTxIdleTime
(
    IN  GT_QD_DEV         *dev,
    OUT GT_U8           *idleTime
);

/*******************************************************************************
* gsysSetEEETimeRate
*
* DESCRIPTION:
*    Set EEE Timer rate:
*                PowerCtrl Timer. 
*                Assertion Timer Rate. 
*                Wake Timer Rate.
*                TxIdle Timer Rate.
*
* INPUTS:
*        eeeTimeRate - EEE  Time rate.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysSetEEETimeRate
(
    IN  GT_QD_DEV         *dev,
    IN  GT_EEE_TIME_RATE  *eeeTimeRate
);

/*******************************************************************************
* gsysGetEEETimeRate
*
* DESCRIPTION:
*    Set EEE Timer rate:
*                PowerCtrl Timer. 
*                Assertion Timer Rate. 
*                Wake Timer Rate.
*                TxIdle Timer Rate.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        eeeTimeRate - EEE  Time rate.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGetEEETimeRate
(
    IN  GT_QD_DEV         *dev,
    OUT GT_EEE_TIME_RATE  *eeeTimeRate
);

/*******************************************************************************
* gsysSetPowerMonitorCtrl
*
* DESCRIPTION:
*    Set Power Monitor Control:
*                Disable the Reset Monitor. 
*                Disable the Voltage Monitor..
*                Reset Time.
*
* INPUTS:
*        pwMonitCtrl - Power Monitor Control structure.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysSetPowerMonitorCtrl
(
    IN  GT_QD_DEV         *dev,
    IN  GT_PW_MONIT_CTRL  *pwMonitCtrl
);

/*******************************************************************************
* gsysGetPowerMonitorCtrl
*
* DESCRIPTION:
*    Get Power Monitor Control:
*                Disable the Reset Monitor. 
*                Disable the Voltage Monitor..
*                Reset Time.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        pwMonitCtrl - Power Monitor Control structure.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGetPowerMonitorCtrl
(
    IN  GT_QD_DEV         *dev,
    OUT GT_PW_MONIT_CTRL  *pwMonitCtrl
);

/*******************************************************************************
* gsysSetPowerMonitorThreshold
*
* DESCRIPTION:
*    Set Power Monitor Threshold:
*                AVDD Voltage Trip Threshold. 
*                Core Voltage Trip Threshold.
*                I/O Voltate Selection range.
*                I/O Voltage Trip Threshold.
*
* INPUTS:
*        pwMonitThresh - Power Monitor Threshold structure.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysSetPowerMonitorThreshold
(
    IN  GT_QD_DEV         *dev,
    IN  GT_PW_MONIT_THRESHOLD  *pwMonitThresh
);

/*******************************************************************************
* gsysGetPowerMonitorThreshold
*
* DESCRIPTION:
*    Get Power Monitor Threshold:
*                AVDD Voltage Trip Threshold. 
*                Core Voltage Trip Threshold.
*                I/O Voltate Selection range.
*                I/O Voltage Trip Threshold.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        pwMonitThresh - Power Monitor Threshold structure.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGetPowerMonitorThreshold
(
    IN  GT_QD_DEV         *dev,
    OUT GT_PW_MONIT_THRESHOLD  *pwMonitThresh
);

/*******************************************************************************
* gsysSetVoltageRegulatCtrl
*
* DESCRIPTION:
*    Set Voltage Regulator Control:
*                1.5 Volt Regulator Selection range. 
*                1.1 Volt Regulator Selection range.
*
* INPUTS:
*        voltRegulCtrl - Voltage Regulator Control structure.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysSetVoltageRegulatCtrl
(
    IN  GT_QD_DEV         *dev,
    IN  GT_VOLT_REGUL_CTRL  *voltRegulCtrl
);

/*******************************************************************************
* gsysGetVoltageRegulatCtrl
*
* DESCRIPTION:
*    Get Voltage Regulator Control:
*                1.5 Volt Regulator Selection range. 
*                1.1 Volt Regulator Selection range.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        voltRegulCtrl - Voltage Regulator Control structure.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGetVoltageRegulatCtrl
(
    IN  GT_QD_DEV         *dev,
    OUT GT_VOLT_REGUL_CTRL  *voltRegulCtrl
);

/*******************************************************************************
* gsysSetVoltageRegulatCtrl_2
*
* DESCRIPTION:
*    Set Voltage Regulator Control 2:
*                Delay. Change the delay between switch control signals in SCR. 
*                Dither. Dither is enabled when this bit is set to a one.
*                Select Clock. Select the internal clock frequency.
*                Select K. Select the feed forward coefficient.
*                Select N. Select the integrator size.
*
* INPUTS:
*        voltRegulCtrl_2 - Voltage Regulator Control 2 structure.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysSetVoltageRegulatCtrl_2
(
    IN  GT_QD_DEV         *dev,
    IN  GT_VOLT_REGUL_CTRL_2  *voltRegulCtrl_2
);

/*******************************************************************************
* gsysGetVoltageRegulatCtrl_2
*
* DESCRIPTION:
*    Get Voltage Regulator Control 2:
*                Delay. Change the delay between switch control signals in SCR. 
*                Dither. Dither is enabled when this bit is set to a one.
*                Select Clock. Select the internal clock frequency.
*                Select K. Select the feed forward coefficient.
*                Select N. Select the integrator size.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        voltRegulCtrl_2 - Voltage Regulator Control 2 structure.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGetVoltageRegulatCtrl_2
(
    IN  GT_QD_DEV         *dev,
    OUT GT_VOLT_REGUL_CTRL_2  *voltRegulCtrl_2
);

/*******************************************************************************
* gsysSetVoltageRegulatTest
*
* DESCRIPTION:
*    Set Voltage Regulator Test:
*                SFGD enable. Enable the safe guard circuit when set to a one. 
*                Test enable. Enable test when this bit is set to a one.
*                Test Select. Selects the test point.
*
* INPUTS:
*        voltRegulTest - Voltage Regulator Test structure.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysSetVoltageRegulatTest
(
    IN  GT_QD_DEV         *dev,
    IN  GT_VOLT_REGUL_TEST  *voltRegulTest
);

/*******************************************************************************
* gsysGetVoltageRegulatTest
*
* DESCRIPTION:
*    Get Voltage Regulator Test:
*                SFGD enable. Enable the safe guard circuit when set to a one. 
*                Test enable. Enable test when this bit is set to a one.
*                Test Select. Selects the test point.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        voltRegulTest - Voltage Regulator Test structure.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGetVoltageRegulatTest
(
    IN  GT_QD_DEV         *dev,
    OUT GT_VOLT_REGUL_TEST  *voltRegulTest
);

/*******************************************************************************
* gsysGlobal2PointDataSet
*
* DESCRIPTION:
*        Common set indirect register pointer control data from the Global2 
*        function registers. The function register are.
*          Scratch & Misc
*          Energy Management
*          IMP Comm/Debug
*          Watch Dog Control
*          QoS Weights
*
* INPUTS:
*        funcReg - Global 2 function register.
*        point - Pointer to following Control register.
*          Scratch & Misc
*          Energy Management
*          IMP Comm/Debug
*          Watch Dog Control
*          QoS Weights
*        data  - Scratch and Misc. Control data written to the register 
*                pointed to by the point above.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGlobal2PointDataSet
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8             funcReg,
    IN  GT_U8             point,
    IN  GT_U8             data
);

/*******************************************************************************
* gsysGlobal2PointDataGet
*
* DESCRIPTION:
*        Common get indirect register pointer control data from the Global2 
*        function registers. The function register are.
*          Scratch & Misc
*          Energy Management
*          IMP Comm/Debug
*          Watch Dog Control
*          QoS Weights
*
* INPUTS:
*        funcReg - Global 2 function register.
*        point - Pointer to following Control register.
*          Scratch & Misc
*          Energy Management
*          IMP Comm/Debug
*          Watch Dog Control
*          QoS Weights
* OUTPUTS:
*        data -  Control data read from the indirect register 
*                pointed to by the point above.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGlobal2PointDataGet
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8             funcReg,
    IN  GT_U8             point,
    OUT  GT_U8            *data
);

/*******************************************************************************
* gdevSetScratchMiscCtrl
*
* DESCRIPTION:
*        Set Scratch and Misc control data to the Scratch and Misc Control register. 
*        The registers of Scratch and Misc control are.
*                Scratch Byte 0
*                Scratch Byte 1
*                GPIO Configuration
*                Reserved for future use
*                GPIO Direction
*                GPIO Data
*                CONFIG Data 0
*                CONFIG Data 1
*                CONFIG Data 2
*                CONFIG Data 3
*                SyncE & TAICLK125�s Drive
*                P5�s & CLK125�s Clock Drive
*                P6�s Clock Drive
*                EEPROM Pad drive
*
* INPUTS:
*        point - Pointer to the Scratch and Misc. Control register.
*        data  - Scratch and Misc. Control data written to the register 
*                pointed to by the point above.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gdevSetScratchMiscCtrl
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8            point,
    IN  GT_U8            data
);

/*******************************************************************************
* gdevGetScratchMiscCtrl
*
* DESCRIPTION:
*        Get Scratch and Misc control data from the Scratch and Misc Control register. 
*        The register of Scratch and Misc control are.
*                Scratch Byte 0
*                Scratch Byte 1
*                GPIO Configuration
*                Reserved for future use
*                GPIO Direction
*                GPIO Data
*                CONFIG Data 0
*                CONFIG Data 1
*                CONFIG Data 2
*                CONFIG Data 3
*                SyncE & TAICLK125�s Drive
*                P5�s & CLK125�s Clock Drive
*                P6�s Clock Drive
*                EEPROM Pad drive

*
* INPUTS:
*        point - Pointer to the Scratch and Misc. Control register.
*
* OUTPUTS:
*        data - Scratch and Misc. Control data read from the register 
*                pointed to by the point above.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gdevGetScratchMiscCtrl
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8             point,
    OUT  GT_U8            *data
);

/*******************************************************************************
* gdevSetEnergeManagementCtrl
*
* DESCRIPTION:
*        Set Energe management control data to the Energe management Control register. 
*        The registers of Energe management control are.
*            Per port EEE receive status counters  
*            EEE status registers � Ports 0 to 15 
*            Per port EEE transmit status counters 
*            EEE status registers � Ports 16 to 26 
*
* INPUTS:
*        point - Pointer to the Energe management Control register.
*        data  - Energe management Control data written to the register 
*                pointed to by the point above.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gdevSetEnergeManagementCtrl
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8             point,
    IN  GT_U8            data
);

/*******************************************************************************
* gdevGetEnergeManagementCtrl
*
* DESCRIPTION:
*        Get Energe management control data from the Energe management Control register. 
*        The register of Energe management control are.
*            Per port EEE receive status counters  
*            EEE status registers � Ports 0 to 15 
*            Per port EEE transmit status counters 
*            EEE status registers � Ports 16 to 26 

*
* INPUTS:
*        point - Pointer to the Energe management Control register.
*
* OUTPUTS:
*        data - Energe management Control data read from the register 
*                pointed to by the point above.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gdevGetEnergeManagementCtrl
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8             point,
    OUT  GT_U8            *data
);

/*******************************************************************************
* gdevSetImpCommCtrl
*
* DESCRIPTION:
*        Set IMP Communication/Debug control data from the IMP Communication/Debug Control register. 
*        The register of IMP Communication/Debug control are.
*           Communication Interface to/from the CPU 
*           Debug Control & Status 
*           CPU�s Main and Alternate Registers 
*           CPU�s Special Purpose Registers 
*           Hidden Debug Pointer 
*           Hidden Debug Data 
*           IMP I/O Page Register 
*           Break Point Control 
*           Break Point Status 
*           Break Point 0 
*           Break Point 1 
*           Break Point 2 
*           Break Point 3 
*           IMP I/O Registers 
*
* INPUTS:
*        point - Pointer to the IMP Communication/Debug Control register.
*        data  - IMP Communication/Debug Control data written to the register 
*                pointed to by the point above.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gdevSetImpCommCtrl
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8             point,
    IN  GT_U8            data
);

/*******************************************************************************
* gdevGetImpCommCtrl
*
* DESCRIPTION:
*        Get IMP Communication/Debug control data from the IMP Communication/Debug Control register. 
*        The register of IMP Communication/Debug control are.
*           Communication Interface to/from the CPU 
*           Debug Control & Status 
*           CPU�s Main and Alternate Registers 
*           CPU�s Special Purpose Registers 
*           Hidden Debug Pointer 
*           Hidden Debug Data 
*           IMP I/O Page Register 
*           Break Point Control 
*           Break Point Status 
*           Break Point 0 
*           Break Point 1 
*           Break Point 2 
*           Break Point 3 
*           IMP I/O Registers 
*
* INPUTS:
*        point - Pointer to the IMP Communication/Debug register.
*
* OUTPUTS:
*        data - IMP Communication/Debug Control data read from the register 
*                pointed to by the point above.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gdevGetImpCommCtrl
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8             point,
    OUT  GT_U8            *data
);

/*******************************************************************************
* gdevSetWatchDogCtrl
*
* DESCRIPTION:
*        Set Watch Dog control data from the Watch Dog Control register. 
*        The register of Watch Dog control are.
*          Watch Dog Interrupt Source 
*          Data Path Watch Dog Interrupts, Masks, Events & History 
*          Policy Interrupts, Masks, Events & History 
*          Auto Fixing Enables 
*
* INPUTS:
*        point - Pointer to the Watch Dog register.
*        data  - Watch Dog Control data written to the register 
*                pointed to by the point above.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gdevSetWatchDogCtrl
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8             point,
    IN  GT_U8            data
);

/*******************************************************************************
* gdevGetWatchDogCtrl
*
* DESCRIPTION:
*        Get Watch Dog control data from the Watch Dog Control register. 
*        The register of Watch Dog control are.
*          Watch Dog Interrupt Source 
*          Data Path Watch Dog Interrupts, Masks, Events & History 
*          Policy Interrupts, Masks, Events & History 
*          Auto Fixing Enables 
*
* INPUTS:
*        point - Pointer to the Watch Dog Control register.
*
* OUTPUTS:
*        data - Watch Dog. Control data read from the register 
*                pointed to by the point above.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gdevGetWatchDogCtrl
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8             point,
    OUT  GT_U8            *data
);

/*******************************************************************************
* gdevSetQosWeightCtrl
*
* DESCRIPTION:
*        Set QoS Weight control data from the QoS Weight Control register. 
*        The register of QoS Weight control are one of 64 possible QoS Weight 
*        Data registers and the QoS Weight Length register.
*         
* INPUTS:
*        point - Pointer to the QoS Weight register.
*        data  - QoS Weight Control data written to the register 
*                pointed to by the point above.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gdevSetQosWeightCtrl
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8             point,
    IN  GT_U8            data
);

/*******************************************************************************
* gdevGetQosWeightCtrl
*
* DESCRIPTION:
*        Set QoS Weight control data from the QoS Weight Control register. 
*        The register of QoS Weight control are one of 64 possible QoS Weight 
*        Data registers and the QoS Weight Length register.
*         
* INPUTS:
*        point - Pointer to the QoS Weight register.
*
* OUTPUTS:
*        data  - QoS Weight Control data written to the register 
*                pointed to by the point above.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gdevGetQosWeightCtrl
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8             point,
    OUT  GT_U8            *data
);

/* gsysGlobal1PointDataSet  */

/*******************************************************************************
* gsysGlobal1PointDataSet
*
* DESCRIPTION:
*        Common set indirect register pointer control data from the Global1 
*        function registers. The function register are.
*          Monitor and Management Control
*
* INPUTS:
*        funcReg - Global 1 function register.
*        point - Pointer to following Control register.
*          Monitor and Management Control
*
*        data  - Scratch and Misc. Control data written to the register 
*                pointed to by the point above.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGlobal1PointDataSet
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8             funcReg,
    IN  GT_U8             point,
    IN  GT_U8             data
);

/*******************************************************************************
* gsysGlobal1PointDataGet
*
* DESCRIPTION:
*        Common get indirect register pointer control data from the Global1 
*        function registers. The function register are.
*          Monitor and Management Control
*
* INPUTS:
*        funcReg - Global 1 function register.
*        point - Pointer to following Control register.
*          Monitor and Management Control
*
* OUTPUTS:
*        data -  Control data read from the indirect register 
*                pointed to by the point above.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGlobal1PointDataGet
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8             funcReg,
    IN  GT_U8             point,
    OUT  GT_U8            *data
);

/*******************************************************************************
* gsysSetMonitorMgmtCtrl
*
* DESCRIPTION:
*        Set Monitor and mgmt control data. 
*         
* INPUTS:
*        point - Pointer to the Monitor and mgmt control register.
*           The Pointer bits are used to access the Index registers as follows:
*              0x00 to 0x01: Rsvd2Cpu Enables for 01:C2:80:00:00:0x
*              0x02 to 0x03: Rsvd2Cpu Enables for 01:C2:80:00:00:2x
*              0x04 to 0x05: Rsvd2IntCpu Enables for 01:C2:80:00:00:0x
*              0x06 to 0x07: Rsvd2IntCpu Enables for 01:C2:80:00:00:2x
*              0x04 to 0x1F: Reserved for future use.
*              0x20 to 0x2F: Mirror Destination Port Settings
*              0x30 to 0x3F: Trap Destination Port Settings
*        data  - Monitor and mgmt Control data written to the register 
*                pointed to by the point above.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysSetMonitorMgmtCtrl
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8             point,
    IN  GT_U8            data
);

/*******************************************************************************
* gsysGetMonitorMgmtCtrl
*
* DESCRIPTION:
*        Get Monitor and mgmt control data. 
*         
* INPUTS:
*        point - Pointer to the Monitor and mgmt control register.
*           The Pointer bits are used to access the Index registers as follows:
*              0x00 to 0x01: Rsvd2Cpu Enables for 01:C2:80:00:00:0x
*              0x02 to 0x03: Rsvd2Cpu Enables for 01:C2:80:00:00:2x
*              0x04 to 0x05: Rsvd2IntCpu Enables for 01:C2:80:00:00:0x
*              0x06 to 0x07: Rsvd2IntCpu Enables for 01:C2:80:00:00:2x
*              0x04 to 0x1F: Reserved for future use.
*              0x20 to 0x2F: Mirror Destination Port Settings
*              0x30 to 0x3F: Trap Destination Port Settings
*
* OUTPUTS:
*        data  - Monitor and mgmt Control data written to the register 
*                pointed to by the point above.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGetMonitorMgmtCtrl
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8             point,
    OUT  GT_U8            *data
);


/*******************************************************************************
* gsysGetSyncETai
*
* DESCRIPTION:
*        Get SyncE and Tai from the Scratch and Misc Control register <SyncE and TAI pad>.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        zpr - ZPR for SyncE and TAI
*        znr - ZNR for SyncE and TAI*
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGetSyncETai
(
    IN  GT_QD_DEV         *dev,
    OUT  GT_U8            *zpr,
    OUT  GT_U8            *znr
);

/*******************************************************************************
* gsysSetP6_Clock
*
* DESCRIPTION:
*        Set P6_clock to the Scratch and Misc Control register <P6_Clock pad>.
*
* INPUTS:
*        zpr - ZPR for P6_Clock
*        znr - ZNR for P6_Clock
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysSetP6_Clock
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8            zpr,
    IN  GT_U8            znr
);

/*******************************************************************************
* gsysGetP6_Clock
*
* DESCRIPTION:
*       Get P6_clock from the Scratch and Misc Control register <P6_Clock pad>.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        zpr - ZPR for P6_Clock
*        znr - ZNR for P6_Clock
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGetP6_Clock
(
    IN  GT_QD_DEV         *dev,
    OUT  GT_U8            *zpr,
    OUT  GT_U8            *znr
);

/*******************************************************************************
* gsysSetP5_Clock
*
* DESCRIPTION:
*       Set P5_clock to the Scratch and Misc Control register <P5_Clock pad>.
*
* INPUTS:
*        zpr - ZPR for P5_Clock
*        znr - ZNR for P5_Clock
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysSetP5_Clock
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8            zpr,
    IN  GT_U8            znr
);

/*******************************************************************************
* gsysGetP5_Clock
*
* DESCRIPTION:
*       Get P5_clock from the Scratch and Misc Control register <P5_Clock pad>.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        zpr - ZPR for P5_Clock
*        znr - ZNR for P5_Clock
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGetP5_Clock
(
    IN  GT_QD_DEV         *dev,
    OUT  GT_U8            *zpr,
    OUT  GT_U8            *znr
);

/*******************************************************************************
* gsysSetEEPROM
*
* DESCRIPTION:
*       Set EEPROM cycle to the Scratch and Misc Control register <EEPROM pad>.
*
* INPUTS:
*        dsm - DSM for EEPROM cycle
*        zpr - ZPR for EEPROM cycle
*        znr - ZNR for EEPROM cycle
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysSetEEPROM
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8            dsm,
    IN  GT_U8            zpr,
    IN  GT_U8            znr
);

/*******************************************************************************
* gsysGetEEPROM
*
* DESCRIPTION:
*       Get EEPROM cycle to the Scratch and Misc Control register <EEPROM pad>.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        dsm - DSM for EEPROM cycle
*        zpr - ZPR for EEPROM cycle
*        znr - ZNR for EEPROM cycle
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysGetEEPROM
(
    IN  GT_QD_DEV         *dev,
    OUT  GT_U8            *dsm,
    OUT  GT_U8            *zpr,
    OUT  GT_U8            *znr
);


/* gtAdvVct.c */

/*******************************************************************************
* gvctGetAdvCableDiag
*
* DESCRIPTION:
*       This routine perform the advanced virtual cable test for the requested
*       port and returns the the status per MDI pair.
*
* INPUTS:
*       port - logical port number.
*       mode - advance VCT mode (either First Peak or Maximum Peak)
*
* OUTPUTS:
*       cableStatus - the port copper cable status.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       Internal Gigabit Phys in 88E6165 family and 88E6351 family devices
*        are supporting this API.
*
*******************************************************************************/
GT_STATUS gvctGetAdvCableDiag
(
    IN  GT_QD_DEV *dev,
    IN  GT_LPORT        port,
    IN  GT_ADV_VCT_MODE mode,
    OUT GT_ADV_CABLE_STATUS *cableStatus
);

/*******************************************************************************
* gvctGetAdvExtendedStatus
*
* DESCRIPTION:
*        This routine retrieves extended cable status, such as Pair Poloarity,
*        Pair Swap, and Pair Skew. Note that this routine will be success only
*        if 1000Base-T Link is up.
*        DSP based cable length is also provided.
*
* INPUTS:
*       dev  - pointer to GT driver structure returned from mdLoadDriver
*       port - logical port number.
*
* OUTPUTS:
*       extendedStatus - the extended cable status.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*        Supporting Device list:
*           88E1111, 88E1112, 88E1141~6, 88E1149, and Internal Gigabit Phys 
*            in 88E6165 family and 88E6351 family devices
*
*******************************************************************************/
GT_STATUS gvctGetAdvExtendedStatus
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT   port,
    OUT GT_ADV_EXTENDED_STATUS *extendedStatus
);

/* gtTCAM.c */

/*******************************************************************************
* gprtSetTcamMode
*
* DESCRIPTION:
*        Set TCAM mode. The following mode is supported:
*            TCAM_MODE_DISABLE,
*            TCAM_MODE_ENABLE_48,
*            TCAM_MODE_ENABLE_96,
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TCAM_MODE type
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if mode is invalid
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtSetTcamMode
(
    IN GT_QD_DEV       *dev,
    IN GT_LPORT        port,
    IN GT_TCAM_MODE    mode
);

/*******************************************************************************
* gprtGetTcamMode
*
* DESCRIPTION:
*        Get TCAM mode. The following mode is supported:
*            TCAM_MODE_DISABLE,
*            TCAM_MODE_ENABLE_48,
*            TCAM_MODE_ENABLE_96,
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode - GT_TCAM_MODE type
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gprtGetTcamMode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_TCAM_MODE    *mode
);

/*******************************************************************************
* gtcamFlushAll
*
* DESCRIPTION:
*       This routine is to flush all entries. A Flush All command will initialize 
*       TCAM Pages 0 and 1, offsets 0x02 to 0x1B to 0x0000, and TCAM Page 2 offset
*       0x02 to 0x05 to 0x0000 for all TCAM entries with the exception that TCAM 
*       Page 0 offset 0x02 will be initialized to 0x00FF.
*
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtcamFlushAll
(
    IN  GT_QD_DEV     *dev
);

/*******************************************************************************
* gtcamFlushEntry
*
* DESCRIPTION:
*       This routine is to flush a single entry. A Flush a single TCAM entry command 
*       will write the same values to a TCAM entry as a Flush All command, but it is
*       done to the selected single TCAM entry only.
*
*
* INPUTS:
*        tcamPointer - pointer to the desired entry of TCAM (0 ~ 254)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtcamFlushEntry
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U32        tcamPointer
);

/*******************************************************************************
* gtcamLoadEntry
*
* DESCRIPTION:
*       This routine loads a TCAM entry.
*        The load sequence of TCAM entry is critical. Each TCAM entry is made up of
*       3 pages of data. All 3 pages need to loaded in a particular order for the TCAM 
*       to operate correctly while frames are flowing through the switch. 
*       If the entry is currently valid, it must first be flushed. Then page 2 needs 
*       to be loaded first, followed by page 1 and then finally page 0. 
*       Each page load requires its own write TCAMOp with these TCAM page bits set 
*       accordingly. 
*
* INPUTS:
*        tcamPointer - pointer to the desired entry of TCAM (0 ~ 254)
*        tcamData    - Tcam entry Data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtcamLoadEntry
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U32        tcamPointer,
    IN  GT_TCAM_DATA        *tcamData
);

/*******************************************************************************
* gtcamPurgyEntry
*
* DESCRIPTION:
*       This routine Purgy a TCAM entry.
*
* INPUTS:
*        tcamPointer - pointer to the desired entry of TCAM (0 ~ 254)
*        tcamData    - Tcam entry Data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtcamPurgyEntry
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U32        tcamPointer,
    IN  GT_TCAM_DATA        *tcamData
);

/*******************************************************************************
* gtcamReadTCAMData
*
* DESCRIPTION:
*       This routine loads the global 3 offsets 0x02 to 0x1B registers with 
*       the data found in the TCAM entry and its TCAM page pointed to by the TCAM
*       entry and TCAM page bits of this register (bits 7:0 and 11:10 respectively.
*
*
* INPUTS:
*        tcamPointer - pointer to the desired entry of TCAM (0 ~ 254)
*
* OUTPUTS:
*        tcamData    - Tcam entry Data
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtcamReadTCAMData
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U32        tcamPointer,
    OUT GT_TCAM_DATA        *tcamData
);

/*******************************************************************************
* gtcamGetNextTCAMData
*
* DESCRIPTION:
*       This routine  finds the next higher TCAM Entry number that is valid (i.e.,
*       any entry whose Page 0 offset 0x02 is not equal to 0x00FF). The TCAM Entry 
*       register (bits 7:0) is used as the TCAM entry to start from. To find 
*       the lowest number TCAM Entry that is valid, start the Get Next operation 
*       with TCAM Entry set to 0xFF.
*
*
* INPUTS:
*        tcamPointer - pointer to the desired entry of TCAM (0 ~ 255)
*
* OUTPUTS:
*        tcamPointer - next pointer entry of TCAM (0 ~ 255)
*        tcamData    - Tcam entry Data
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtcamGetNextTCAMData
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U32        *tcamPointer,
    OUT GT_TCAM_DATA        *tcamData
);

/* Serdes API */
/*******************************************************************************
* gprtGetSerdesMode
*
* DESCRIPTION:
*       This routine reads Serdes Interface Mode.
*
* INPUTS:
*        port -  The serdessical SERDES device address(4/5)
*
* OUTPUTS:
*       mode    - Serdes Interface Mode
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       logical port number is supported only for the devices made production 
*       before 2009. 
*  (Serdes devices: 88E6131, 88E6122, 88E6108, 88E6161, 88E6165, 88E6352 and 88E320 family)
*
*******************************************************************************/
GT_STATUS gprtGetSerdesMode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_SERDES_MODE *mode
);

/*******************************************************************************
* gprtSetSerdesMode
*
* DESCRIPTION:
*       This routine sets Serdes Interface Mode.
*
* INPUTS:
*       port -  The serdessical SERDES device address(4/5)
*       mode    - Serdes Interface Mode
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       logical port number is supported only for the devices made production 
*       before 2009. 
*  (Serdes devices: 88E6131, 88E6122, 88E6108, 88E6161, 88E6165 family)
*
*******************************************************************************/
GT_STATUS gprtSetSerdesMode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_SERDES_MODE mode
);

/* Set SERDES Register. */
/*******************************************************************************
* gprtSetSerdesReg
*
* DESCRIPTION:
*       This routine writes Phy Serdes Registers.
*
* INPUTS:
*       port -    The logical port number.
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetSerdesReg
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT        port,
    IN  GT_U32            regAddr,
    IN  GT_U16            data
);

/* Get SERDES Register. */
/*******************************************************************************
* gprtGetSerdesReg
*
* DESCRIPTION:
*       This routine reads Phy Serdes Registers.
*
* INPUTS:
*       port -    The logical port number.
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetSerdesReg
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_U32         regAddr,
    OUT GT_U16         *data
);

/*******************************************************************************
* gprtSetSerdesRegField
*
* DESCRIPTION:
*		This routine sets Serdes Register.
*
* INPUTS:
*       portNum     - Port number to write the register for.
*       regAddr     - The register's address.
*       fieldOffset - The field start bit index. (0 - 15)
*       fieldLength - Number of bits to write.
*       data        - Data to be written.	
*
* OUTPUTS:
*		 None.
*
* RETURNS:
*		GT_OK			- on success
*		GT_FAIL 		- on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gprtSetSerdesRegField
(
IN GT_QD_DEV *dev,
    IN  GT_LPORT  port,
    IN  GT_U8    regAddr,
    IN  GT_U8    fieldOffset,
    IN  GT_U8    fieldLength,
    IN  GT_U16   data
);

/* Get SERDES Register. */
/*******************************************************************************
* gprtGetSerdesRegField
*
* DESCRIPTION:
*		This routine gets Serdes Register.
*
* INPUTS:
*       portNum     - Port number to write the register for.
*       regAddr     - The register's address.
*       fieldOffset - The field start bit index. (0 - 15)
*       fieldLength - Number of bits to write.
*
* OUTPUTS:
*       data        - Data to be read.	
*
* RETURNS:
*		GT_OK			- on success
*		GT_FAIL 		- on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gprtGetSerdesRegField
(
IN GT_QD_DEV *dev,
    IN  GT_LPORT  port,
    IN  GT_U8    regAddr,
    IN  GT_U8    fieldOffset,
    IN  GT_U8    fieldLength,
    OUT  GT_U16   *data
);

/*******************************************************************************
* gprtSetSerdesIntEnable
*
* DESCRIPTION:
*       This routine set Serdes interrupt enable.
*
* INPUTS:
*       port -    The logical port number.
*       data    - Interrupt bits.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetSerdesIntEnable 
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT        port,
    IN  GT_U16         data
);

/*******************************************************************************
* gprtGetSerdesIntEnable
*
* DESCRIPTION:
*       This routine get Serdes interrupt enable setting.
*
* INPUTS:
*       port -    The logical port number.
*
* OUTPUTS:
*       data    - Interrupt bits.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetSerdesIntEnable 
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT        port,
    OUT  GT_U16         *data
);

/*******************************************************************************
* gprtGetSerdesIntStatus
*
* DESCRIPTION:
*       This routine get Serdes interrupt status and clear the status.
*
* INPUTS:
*       port -    The logical port number.
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The interrupt status.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetSerdesIntStatus 
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT        port,
    OUT  GT_U16          *data
);

/*******************************************************************************
* gprtSerdesReset
*
* DESCRIPTION:
*       This routine preforms SERDES reset.
*        After reset, serdes will be in Autonegotiation mode.
*
* INPUTS:
* port - The logical port number, unless SERDES device is accessed
*        The serdessical address, if SERDES device is accessed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
* COMMENTS:
* data sheet register 0.15 - Reset
* data sheet register 0.13 - Speed
* data sheet register 0.12 - Autonegotiation
* data sheet register 0.8  - Duplex Mode
*******************************************************************************/

GT_STATUS gprtSerdesReset
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT  port
);

/*******************************************************************************
* gprtSetSerdesLoopback
*
* DESCRIPTION:
* Enable/Disable Internal Serdes Loopback. 
*
* INPUTS:
* port - The logical port number, unless SERDES device is accessed
*        The physical address, if SERDES device is accessed
* enable - If GT_TRUE, enable loopback mode
* If GT_FALSE, disable loopback mode
*
* OUTPUTS:
* None.
*
* RETURNS:
* GT_OK - on success
* GT_FAIL - on error
*
* COMMENTS:
* data sheet register 0.14 - Loop_back
*
*******************************************************************************/

GT_STATUS gprtSetSerdesLoopback
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT  port,
    IN GT_BOOL   enable
);

/*******************************************************************************
* gprtGetSerdesLoopback
*
* DESCRIPTION:
* Gets status of Enable/Disable Internal Serdes Loopback. 
*
* INPUTS:
* port - The logical port number, unless SERDES device is accessed
*        The physical address, if SERDES device is accessed
*
* OUTPUTS:
* enable - If GT_TRUE, enable loopback mode
* If GT_FALSE, disable loopback mode
*
* RETURNS:
* GT_OK - on success
* GT_FAIL - on error
*
* COMMENTS:
* data sheet register 0.14 - Loop_back
*
*******************************************************************************/

GT_STATUS gprtGetSerdesLoopback
(
    IN  GT_QD_DEV *dev,
    IN  GT_LPORT  port,
    OUT GT_BOOL   *enable
);

/*******************************************************************************
* gprtSetSerdesSpeed
*
* DESCRIPTION:
*         Sets speed for a specific logical port. This function will keep the duplex 
*        mode and loopback mode to the previous value, but disable others, such as 
*        Autonegotiation.
*
* INPUTS:
*        port -    The logical port number, unless SERDES device is accessed
*                The serdessical address, if SERDES device is accessed
*        speed - port speed.
*                PHY_SPEED_10_MBPS for 10Mbps
*                PHY_SPEED_100_MBPS for 100Mbps
*                PHY_SPEED_1000_MBPS for 1000Mbps
*
* OUTPUTS:
* None.
*
* RETURNS:
* GT_OK - on success
* GT_FAIL - on error
*
* COMMENTS:
* data sheet register 0.13 - Speed Selection (LSB)
* data sheet register 0.6  - Speed Selection (MSB)
*
*******************************************************************************/

GT_STATUS gprtSetSerdesSpeed
(
IN GT_QD_DEV *dev,
IN GT_LPORT  port,
IN GT_PHY_SPEED speed
);

/*******************************************************************************
* gprtSerdesAutoNegEnable
*
* DESCRIPTION:
*         Enable/disable an Auto-Negotiation.
*        This routine simply sets Auto Negotiation bit (bit 12) of Control 
*        Register and reset the serdes.
*        For Speed and Duplex selection, please use gprtSetSerdesAutoMode.
*
* INPUTS:
*        port -  The logical port number, unless SERDES device is accessed
*                The serdessical address, if SERDES device is accessed
*         state - GT_TRUE for enable Auto-Negotiation,
*                GT_FALSE otherwise
*
* OUTPUTS:
*         None.
*
* RETURNS:
*         GT_OK     - on success
*         GT_FAIL     - on error
*
* COMMENTS:
*         data sheet register 0.12 - Auto-Negotiation Enable
*         data sheet register 4.8, 4.7, 4.6, 4.5 - Auto-Negotiation Advertisement
*
*******************************************************************************/
GT_STATUS gprtSerdesAutoNegEnable
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT  port,
    IN GT_BOOL   state
);

/*******************************************************************************
* gprtSerdesPowerDown
*
* DESCRIPTION:
*         Enable/disable (power down) on specific logical port.
*        Phy configuration remains unchanged after Power down.
*
* INPUTS:
*        port -    The logical port number, unless SERDES device is accessed
*                The serdessical address, if SERDES device is accessed
*         state -    GT_TRUE: power down
*                 GT_FALSE: normal operation
*
* OUTPUTS:
*         None.
*
* RETURNS:
*         GT_OK     - on success
*         GT_FAIL     - on error
*
* COMMENTS:
*         data sheet register 0.11 - Power Down
*
*******************************************************************************/

GT_STATUS gprtSerdesPowerDown
(
IN GT_QD_DEV *dev,
IN GT_LPORT  port,
IN GT_BOOL   state
);

/*******************************************************************************
* gprtGetSerdesPowerDownSt
*
* DESCRIPTION:
*        Gets status of Enable/disable (power down) on specific logical port.
*        Phy configuration remains unchanged after Power down.
*
* INPUTS:
*        port -    The logical port number, unless SERDES device is accessed
*                The physical address, if SERDES device is accessed
*
* OUTPUTS:
*        state -    GT_TRUE: power down
*                 GT_FALSE: normal operation
*
* RETURNS:
*         GT_OK     - on success
*         GT_FAIL     - on error
*
* COMMENTS:
*         data sheet register 0.11 - Power Down
*
*******************************************************************************/

GT_STATUS gprtGetSerdesPowerDownSt
(
IN GT_QD_DEV *dev,
IN GT_LPORT  port,
OUT GT_BOOL  *state
);

/*******************************************************************************
* gprtSerdesRestartAutoNeg
*
* DESCRIPTION:
*         Restart AutoNegotiation. If AutoNegotiation is not enabled, it'll enable 
*        it. Loopback and Power Down will be disabled by this routine.
*
* INPUTS:
*        port -    The logical port number, unless SERDES device is accessed
*                The serdessical address, if SERDES device is accessed
*
* OUTPUTS:
*         None.
*
* RETURNS:
*         GT_OK     - on success
*         GT_FAIL     - on error
*
* COMMENTS:
*         data sheet register 0.9 - Restart Auto-Negotiation
*
*******************************************************************************/

GT_STATUS gprtSerdesRestartAutoNeg
( 
IN GT_QD_DEV *dev,
IN GT_LPORT  port
);

/*******************************************************************************
* gprtSetSerdesDuplexMode
*
* DESCRIPTION:
*         Sets duplex mode for a specific logical port. This function will keep 
*        the speed and loopback mode to the previous value, but disable others,
*        such as Autonegotiation.
*
* INPUTS:
*        port -    The logical port number, unless SERDES device is accessed
*                The serdessical address, if SERDES device is accessed
*         dMode    - dulpex mode
*
* OUTPUTS:
*         None.
*
* RETURNS:
*         GT_OK     - on success
*         GT_FAIL     - on error
*
* COMMENTS:
*         data sheet register 0.8 - Duplex Mode
*
*******************************************************************************/
GT_STATUS gprtSetSerdesDuplexMode
(
IN GT_QD_DEV *dev,
IN GT_LPORT  port,
IN GT_BOOL   dMode
);

/*******************************************************************************
* gprtSetSerdesAutoMode
*
* DESCRIPTION:
*         This routine sets up the port with given Auto Mode.
*        Supported mode is as follows:
*        - Auto for both speed and duplex.
*        - Auto for speed only and Full duplex.
*        - Auto for speed only and Half duplex.
*        - Auto for duplex only and speed 1000Mbps.
*        - Auto for duplex only and speed 100Mbps.
*        - Auto for duplex only and speed 10Mbps.
*        - Speed 1000Mbps and Full duplex.
*        - Speed 1000Mbps and Half duplex.
*        - Speed 100Mbps and Full duplex.
*        - Speed 100Mbps and Half duplex.
*        - Speed 10Mbps and Full duplex.
*        - Speed 10Mbps and Half duplex.
*        
*
* INPUTS:
*        port -    The logical port number, unless SERDES device is accessed
*                The serdessical address, if SERDES device is accessed
*         mode - Auto Mode to be written
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - on device without copper
*
* COMMENTS:
*         data sheet register 4.8, 4.7, 4.6, and 4.5 Autonegotiation Advertisement
*         data sheet register 4.6, 4.5 Autonegotiation Advertisement for 1000BX
*         data sheet register 9.9, 9.8 Autonegotiation Advertisement for 1000BT
*******************************************************************************/

GT_STATUS gprtSetSerdesAutoMode
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT  port,
    IN GT_PHY_AUTO_MODE mode
);

/*******************************************************************************
* gprtSetSerdesPause
*
* DESCRIPTION:
*       This routine will set the pause bit in Autonegotiation Advertisement
*        Register. And restart the autonegotiation.
*
* INPUTS:
*        port -    The logical port number, unless SERDES device is accessed
*                The serdessical address, if SERDES device is accessed
*        state - GT_SERDES_PAUSE_MODE enum value.
*                GT_SERDES_NO_PAUSE        - disable pause
*                 GT_SERDES_PAUSE        - support pause
*                GT_SERDES_ASYMMETRIC_PAUSE    - support asymmetric pause
*                GT_SERDES_BOTH_PAUSE    - support both pause and asymmetric pause
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
* COMMENTS:
* data sheet register 4.10 Autonegotiation Advertisement Register
*******************************************************************************/

GT_STATUS gprtSetSerdesPause
(
IN GT_QD_DEV *dev,
IN GT_LPORT  port,
IN GT_PHY_PAUSE_MODE state
);

/*******************************************************************************
* gprtGetSerdesLinkStatus
*
* DESCRIPTION:
*       This routine retrieves the Link status.
*
* INPUTS:
*        port -    The logical port number, unless SERDES device is accessed
*                The serdessical address, if SERDES device is accessed
*
* OUTPUTS:
*       linkStatus - GT_FALSE if link is not established,
*                     GT_TRUE if link is established.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*        
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gprtGetSerdesLinkStatus
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT  port,
    IN GT_BOOL      *linkStatus
);


































/*******************************************************************************
* gprtGetSerdesMode
*
* DESCRIPTION:
*       This routine reads Serdes Interface Mode.
*
* INPUTS:
*        port -  The physical SERDES device address(4/5)
*
* OUTPUTS:
*       mode    - Serdes Interface Mode
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       logical port number is supported only for the devices made production 
*       before 2009. 
*  (Serdes devices: 88E6131, 88E6122, 88E6108, 88E6161, 88E6165 and 88E352 family)
*
*******************************************************************************/
GT_STATUS gprtGetSerdesMode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_SERDES_MODE *mode
);

/*******************************************************************************
* gprtGetSerdesReg
*
* DESCRIPTION:
*       This routine reads Phy Serdes Registers.
*
* INPUTS:
*       port -    The logical port number.
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetSerdesReg
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_U32         regAddr,
    OUT GT_U16         *data
);

/*******************************************************************************
* gprtSetSerdesReg
*
* DESCRIPTION:
*       This routine writes Phy Serdes Registers.
*
* INPUTS:
*       port -    The logical port number.
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetSerdesReg
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT        port,
    IN  GT_U32            regAddr,
    IN  GT_U16            data
);
	GT_STATUS gprtSetSerdesRegField  /* */
	(
		IN GT_QD_DEV *dev,
    	IN  GT_LPORT  port,
    	IN  GT_U8    regAddr,
    	IN  GT_U8    fieldOffset,
    	IN  GT_U8    fieldLength,
    	IN  GT_U16   data
	);
	GT_STATUS gprtGetSerdesRegField  /* */
	(
	IN GT_QD_DEV *dev,
    	IN  GT_LPORT  port,
    	IN  GT_U8    regAddr,
    	IN  GT_U8    fieldOffset,
    	IN  GT_U8    fieldLength,
    	OUT GT_U16   *data
	);

/*******************************************************************************
* gprtSetSerdesIntEnable
*
* DESCRIPTION:
*       This routine set Serdes interrupt enable.
*
* INPUTS:
*       port -    The logical port number.
*       data    - Interrupt bits.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetSerdesIntEnable 
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT        port,
    IN  GT_U16         data
);


/*******************************************************************************
* gprtGetSerdesIntEnable
*
* DESCRIPTION:
*       This routine get Serdes interrupt enable setting.
*
* INPUTS:
*       port -    The logical port number.
*
* OUTPUTS:
*       data    - Interrupt bits.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetSerdesIntEnable 
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT        port,
    OUT  GT_U16         *data
);


/*******************************************************************************
* gprtGetSerdesIntStatus
*
* DESCRIPTION:
*       This routine get Serdes interrupt status and clear the status.
*
* INPUTS:
*       port -    The logical port number.
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The interrupt status.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetSerdesIntStatus 
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT        port,
    OUT  GT_U16          *data
);


#ifdef __cplusplus
}
#endif

#endif /* __msApi_h */
