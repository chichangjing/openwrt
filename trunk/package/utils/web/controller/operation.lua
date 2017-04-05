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

module("luci.controller.admin.operation", package.seeall)

function index()
	entry({"admin", "operation"}, alias("admin", "operation",  "syslog"),   _("System operation"), 55).index = true
	entry({"admin", "operation",  "syslog"},     call("action_syslog"),      _("System Log"),     1 )
    entry({"admin", "operation", "dmesg"},       call("action_dmesg"),       _("Kernel Log"), 2)
    entry({"admin", "operation", "admin"},       template("admin_ethernet/system/admin"),  _("Password change"), 3)
    entry({"admin", "operation", "language"},    cbi("admin_ethernet/switchConfig/system"), _("Language change"),4)
    entry({"admin", "operation", "diagnostics"}, template("admin_ethernet/system/diagnostics"), _("Diagnostics"), 5)
    entry({"admin", "operation", "diag_ping"},   call("diag_ping"), nil).leaf = true

    entry({"admin", "operation", "flashops"},   call("action_flashops"), _("Backup / Restore"), 70)
    --entry({"admin", "operation", "flashops", "backupfiles"}, form("admin_ethernet/switchConfig/backupfiles")).leaf = true
end

function action_syslog()
    local syslog = luci.sys.syslog()
    luci.template.render("admin_status/syslog", {syslog=syslog})
end


function action_dmesg()
    local dmesg = luci.sys.dmesg()
    luci.template.render("admin_status/dmesg", {dmesg=dmesg})
end

function diag_command(cmd, addr)
    if addr and addr:match("^[a-zA-Z0-9%-%.:_]+$") then
        luci.http.prepare_content("text/plain")

        local util = io.popen(cmd % addr)
        if util then
            while true do
                local ln = util:read("*l")
                if not ln then break end
                luci.http.write(ln)
                luci.http.write("\n")
            end

            util:close()
        end

        return
    end

    luci.http.status(500, "Bad address")
end

function diag_ping(addr)
    diag_command("ping -c 5 -W 1 %q 2>&1", addr)
end

function action_flashops()
    local sys = require "luci.sys"
    local fs  = require "nixio.fs"

    local upgrade_avail = fs.access("/lib/upgrade/platform.sh")
    local reset_avail   = os.execute([[grep '"rootfs_data"' /proc/mtd >/dev/null 2>&1]]) == 0

    local restore_cmd = "cat > /tmp/res.tgz"
    local backup_cmd  = "sysupgrade --create-backup - 2>/dev/null"
    local image_tmp   = "/tmp/firmware.img"
    local fpga_tmp   = "/tmp/fpga.bin"
    local jw_image   = "/tmp/images/openwrt-imx6ul-generic-jffs2-64k.img"

    local function image_split()
        luci.sys.exec("dd if=%s of=/tmp/p1 bs=1k count=4" % image_tmp)
        luci.sys.exec("dd if=%s of=/tmp/p2 bs=1k skip=4" % image_tmp)
        luci.sys.exec("mkdir -p /tmp/images")
        return luci.sys.exec("tar -zxvf /tmp/p2 -C /tmp/images")
    end

    --local function image_checksum()
    --    return (luci.sys.exec("md5sum %q" % jw_image):match("^([^%s]+)"))
    --end

    local function image_supported()
        image_split()
        --local image_md5 = image_checksum()
        local ret = (luci.sys.exec("/usr/sbin/jwimage_check.py"):match("^([^%s]+)"))
        return (ret == "true")
    end

    local function storage_size()
        local size = 0
        if fs.access("/proc/mtd") then
            for l in io.lines("/proc/mtd") do
                local d, s, e, n = l:match('^([^%s]+)%s+([^%s]+)%s+([^%s]+)%s+"([^%s]+)"')
                if n == "linux" or n == "firmware" then
                    size = tonumber(s, 16)
                    break
                end
            end
        elseif fs.access("/proc/partitions") then
            for l in io.lines("/proc/partitions") do
                local x, y, b, n = l:match('^%s*(%d+)%s+(%d+)%s+([^%s]+)%s+([^%s]+)')
                if b and n and not n:match('[0-9]') then
                    size = tonumber(b) * 1024
                    break
                end
            end
        end
        return size
    end


    local fp
    luci.http.setfilehandler(
        function(meta, chunk, eof)
            if not fp then
                if meta and meta.name == "image" then
                    fp = io.open(image_tmp, "w")
                elseif meta and meta.name == "fpga_get" then
                    fp = io.open(fpga_tmp, "w")
                else
                    fp = io.popen(restore_cmd, "w")
                end
            end
            if chunk then
                fp:write(chunk)
            end
            if eof then
                fp:close()
            end
        end
    )

    if luci.http.formvalue("backup") then
        --
        -- Assemble file list, generate backup
        --
        local reader = ltn12_popen(backup_cmd)
        luci.http.header('Content-Disposition', 'attachment; filename="backup-%s-%s.tar.gz"' % {
            luci.sys.hostname(), os.date("%Y-%m-%d")})
        luci.http.prepare_content("application/x-targz")
        luci.ltn12.pump.all(reader, luci.http.write)
    elseif luci.http.formvalue("restore") then
        --
        -- Unpack received .tar.gz
        --
        local upload = luci.http.formvalue("archive")
        if upload and #upload > 0 then
            luci.template.render("admin_ethernet/system/applyreboot")
            luci.sys.exec("/sbin/sysupgrade -r /tmp/res.tgz")
            luci.sys.reboot()
        end
    elseif luci.http.formvalue("fpga") then
        --
        -- Initiate fpga flash
        --
        local fpgaload = luci.http.formvalue("fpga_get")
        if fpgaload and #fpgaload > 0 then
            --luci.sys.exec("flash_eraseall /dev/mtd5")
            luci.sys.exec("jw_mtd5_erase")
            luci.sys.exec("dd if=%s of=/dev/mtd5 bs=1K seek=8" % fpga_tmp)
            luci.sys.exec("echo 4 > /sys/class/gpio/export")
            luci.sys.exec("echo out > /sys/class/gpio/gpio4/direction")
            luci.sys.exec("echo 1 > /sys/class/gpio/gpio4/value")
            luci.sys.exec("sleep 1")
            luci.sys.exec("echo 0 > /sys/class/gpio/gpio4/value")
            luci.sys.exec("sleep 1")
            luci.sys.reboot()
            luci.template.render("admin_ethernet/system/applyreboot", {
                title = luci.i18n.translate("Flashing..."),
                msg   = luci.i18n.translate("The system is flashing now.<br /> DO NOT POWER OFF THE DEVICE!<br /> Wait a few minutes before you try to reconnect. It might be necessary to renew the address of your computer to reach the device again, depending on your settings."),
                addr  =  nil
            })
        end
    elseif luci.http.formvalue("image") or luci.http.formvalue("step") then
        --
        -- Initiate firmware flash
        --
        local step = tonumber(luci.http.formvalue("step") or 1)
        if step == 1 then
            if image_supported() then
                luci.template.render("admin_ethernet/system/upgrade", {
                    checksum = image_md5,
                    storage  = storage_size(),
                    size     = (fs.stat(jw_image, "size") or 0),
                    keep     = (not not luci.http.formvalue("keep"))
                })
            else
                fs.unlink(image_tmp)
                luci.template.render("admin_ethernet/system/flashops", {
                    reset_avail   = reset_avail,
                    upgrade_avail = upgrade_avail,
                    image_invalid = true
                })
            end
        --
        -- Start sysupgrade flash
        --
        elseif step == 2 then
            local keep = (luci.http.formvalue("keep") == "1") and "" or "-n"
            luci.template.render("admin_ethernet/system/applyreboot", {
                title = luci.i18n.translate("Flashing..."),
                msg   = luci.i18n.translate("The system is flashing now.<br /> DO NOT POWER OFF THE DEVICE!<br /> Wait a few minutes before you try to reconnect. It might be necessary to renew the address of your computer to reach the device again, depending on your settings."),
                addr  = (#keep > 0) and "192.168.1.1" or nil
            })
            --fork_exec("killall dropbear uhttpd; sleep 1; /sbin/sysupgrade -v" %{ image_tmp })
            luci.sys.exec("touch /tmp/web_upgrade")
            luci.sys.exec("/sbin/sysupgrade -v %s" %{ jw_image })
        end
    elseif reset_avail and luci.http.formvalue("reset") then
        --
        -- Reset system
        --
        luci.template.render("admin_ethernet/system/applyreboot", {
            title = luci.i18n.translate("Erasing..."),
            msg   = luci.i18n.translate("The system is erasing the configuration partition now and will reboot itself when finished."),
            addr  = "192.168.1.1"
        })
        fork_exec("killall dropbear uhttpd; sleep 1; mtd -r erase rootfs_data")
    else
        --
        -- Overview
        --
        luci.template.render("admin_ethernet/system/flashops", {
            reset_avail   = reset_avail,
            upgrade_avail = upgrade_avail
        })
    end
end

function ltn12_popen(command)

    local fdi, fdo = nixio.pipe()
    local pid = nixio.fork()

    if pid > 0 then
        fdo:close()
        local close
        return function()
            local buffer = fdi:read(2048)
            local wpid, stat = nixio.waitpid(pid, "nohang")
            if not close and wpid and stat == "exited" then
                close = true
            end

            if buffer and #buffer > 0 then
                return buffer
            elseif close then
                fdi:close()
                return nil
            end
        end
    elseif pid == 0 then
        nixio.dup(fdo, nixio.stdout)
        fdi:close()
        fdo:close()
        nixio.exec("/bin/sh", "-c", command)
    end
end


