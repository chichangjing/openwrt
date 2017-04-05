--[[
LuCI - Lua Configuration Interface

Copyright 2008 Steven Barth <steven@midlink.org>
Copyright 2011 Jo-Philipp Wich <xm@subsignal.org>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

$Id$
]]--

module("luci.controller.admin.ethernet", package.seeall)

function index()
	entry({"admin", "ethernet"}, alias("admin", "ethernet",  "service"), _("Advanced configuration"), 35).index = true
	entry({"admin", "ethernet", "rate_limit"},          template("admin_ethernet/switchConfig/rateLimit"), _("Rate limitation"),     3 )	
    entry({"admin", "ethernet", "mirror_config",  },    template("admin_ethernet/switchConfig/mirrorConfig"), _("Port mirror"), 4 )
	entry({"admin", "ethernet", "QOS_config",  },       template("admin_ethernet/switchConfig/qosConfig"), _("Qos Configuration"), 5 )
	entry({"admin", "ethernet", "fdb_config",  },       template("admin_ethernet/switchConfig/fdbConfig"), _("Port security"), 6 )
	entry({"admin", "ethernet", "trunk_config",  },     template("admin_ethernet/switchConfig/trunkConfig"), _("Port aggregation"), 7 )
    entry({"admin", "ethernet", "multi_config",  },     template("admin_ethernet/switchConfig/multiConfig"), _("Static multicast"), 8 )
	entry({"admin", "ethernet", "vlan_config",  },      template("admin_ethernet/switchConfig/vlanConfig"), _("VLAN configuration"), 9 )
	entry({"admin", "ethernet", "jrpp_config",  },      template("admin_ethernet/switchConfig/redundancyConfig"), _("Ring configuration"), 10 )
	entry({"admin", "ethernet", "lldp_config",  },      template("admin_ethernet/switchConfig/lldpConfig"), _("LLDP configuration"), 11 )
	entry({"admin", "ethernet", "rstp_config",  },      template("admin_ethernet/switchConfig/rstpConfig"), _("RSTP configuration"), 12 )
	entry({"admin", "ethernet", "trap_config",  },      template("admin_ethernet/switchConfig/trapConfig"), _("TRAP configuration"), 13 )

end

