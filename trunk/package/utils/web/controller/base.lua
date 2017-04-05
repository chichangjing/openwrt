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

module("luci.controller.admin.base", package.seeall)

function index()

	entry({"admin", "base_info"}, alias("admin", "base_info",  "sysinfo"), _("Basic configuration"), 25).index = true
	entry({"admin", "base_info",  "sysinfo"},                template("admin_ethernet/system/sysinfo"),        _("Device information"),     1 )
	entry({"admin", "base_info",  "network"},                template("admin_ethernet/system/networkConfig"),  _("Network configuration"),     2 )
	entry({"admin", "base_info",  "snmpconfig"},             template("admin_ethernet/system/snmpConfig"),     _("SNMP Configuration"),     3 )
	entry({"admin", "base_info",  "port_config"},            template("admin_ethernet/switchConfig/portConfig"), _("Port configuration"),   4 )

    entry({"admin", "base_info", "switch_config"}, alias("admin", "base_info", "switch_config", "flow_statistics"), _("Flow statistics"), 25)
    entry({"admin", "base_info", "switch_config",  "flow_statistics"},  template("admin_ethernet/switchConfig/flowConfig"), _("information display"),1 ).leaf = true
    entry({"admin", "base_info", "switch_config",  "bandwidth"},        template("admin_ethernet/switchConfig/bandwidth"),  _("Realtime Graphs"), 2 ).leaf = true
    entry({"admin", "base_info", "switch_config",  "bandwidth_status"}, call("action_bandwidth")).leaf = true

	entry({"admin", "save"}, template("admin_ethernet/switchConfig/saveConfig"), _("Save changes"), 65).index = true
    entry({"admin", "backup"},      template("admin_ethernet/switchConfig/clearConfig"), _("Restore default"), 70)
    entry({"admin", "reboot"}, call("action_reboot"), _("Reboot"), 80)
end

function action_reboot()
    local reboot = luci.http.formvalue("reboot")
    luci.template.render("admin_ethernet/system/reboot", {reboot=reboot})
    if reboot then
        luci.sys.reboot()
    end
end

function action_bandwidth(iface)
    luci.http.prepare_content("application/json")

    local bwc = io.popen("luci-bwc -i %q 2>/dev/null" % iface)
    if bwc then
        luci.http.write("[")

        while true do
            local ln = bwc:read("*l")
            if not ln then break end
            luci.http.write(ln)
        end

        luci.http.write("]")
        bwc:close()
    end
end

