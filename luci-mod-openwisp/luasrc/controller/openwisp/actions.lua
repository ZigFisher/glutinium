-- Copyright 2008 Steven Barth <steven@midlink.org>
-- Copyright 2008 Jo-Philipp Wich <jow@openwrt.org>
-- Re-adapted for the OpenWISP Project (GPLv3) http://openwisp.org

module("luci.controller.openwisp.actions", package.seeall)

function index()
	entry({"openwisp", "actions"}, alias("openwisp", "actions", "index"), _("Actions"), 30).index = true
	entry({"openwisp", "actions", "upgrade"}, call("action_upgrade"), _("Upgrade Firmware"), 31)
	entry({"openwisp", "actions", "sysupgrade"}, call("action_sysupgrade"))
	entry({"openwisp", "actions", "reboot"}, template("openwisp/reboot"), _("Reboot"), 90)
	entry({"openwisp", "actions", "reboot", "call"}, post("action_reboot"))
	entry({"openwisp", "logout"}, call("action_logout"), _("Logout"), 40)
end

function action_reboot()
	luci.sys.reboot()
end

local function image_supported(image)
	return (os.execute("sysupgrade -T %q >/dev/null" % image) == 0)
end

local function image_checksum(image)
	return (luci.sys.exec("md5sum %q" % image):match("^([^%s]+)"))
end

local function image_sha256_checksum(image)
	return (luci.sys.exec("sha256sum %q" % image):match("^([^%s]+)"))
end

local function supports_sysupgrade()
	return nixio.fs.access("/lib/upgrade/platform.sh")
end

local function storage_size()
	local size = 0
	if nixio.fs.access("/proc/mtd") then
		for l in io.lines("/proc/mtd") do
			local d, s, e, n = l:match('^([^%s]+)%s+([^%s]+)%s+([^%s]+)%s+"([^%s]+)"')
			if n == "linux" or n == "firmware" then
				size = tonumber(s, 16)
				break
			end
		end
	elseif nixio.fs.access("/proc/partitions") then
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

function action_upgrade()
	--
	-- Overview
	--
	luci.template.render("openwisp/upgrade/select", {
		upgrade_avail = supports_sysupgrade()
	})
end

function action_sysupgrade()
	local fs = require "nixio.fs"
	local http = require "luci.http"
	local image_tmp = "/tmp/firmware.img"

	local fp
	http.setfilehandler(
		function(meta, chunk, eof)
			if not fp and meta and meta.name == "image" then
				fp = io.open(image_tmp, "w")
			end
			if fp and chunk then
				fp:write(chunk)
			end
			if fp and eof then
				fp:close()
			end
		end
	)

	if not luci.dispatcher.test_post_security() then
		fs.unlink(image_tmp)
		return
	end

	--
	-- Cancel firmware flash
	--
	if http.formvalue("cancel") then
		fs.unlink(image_tmp)
		http.redirect(luci.dispatcher.build_url('openwisp/actions/upgrade'))
		return
	end

	--
	-- Initiate firmware flash
	--
	local step = tonumber(http.formvalue("step") or 1)
	if step == 1 then
		if image_supported(image_tmp) then
			luci.template.render("openwisp/upgrade/confirm", {
				checksum = image_checksum(image_tmp),
				sha256ch = image_sha256_checksum(image_tmp),
				storage  = storage_size(),
				size     = (fs.stat(image_tmp, "size") or 0),
				keep     = (not not http.formvalue("keep"))
			})
		else
			fs.unlink(image_tmp)
			luci.template.render("openwisp/upgrade/select", {
				upgrade_avail = supports_sysupgrade(),
				image_invalid = true
			})
		end
	--
	-- Start sysupgrade flash
	--
	elseif step == 2 then
		local keep = (http.formvalue("keep") == "1") and "" or "-n"
		luci.template.render("openwisp/upgrade/applyreboot", {
			title = luci.i18n.translate("Flashing..."),
			msg   = luci.i18n.translate("The system is flashing now.<br /> DO NOT POWER OFF THE DEVICE!<br /> Wait a few minutes before you try to reconnect. It might be necessary to renew the address of your computer to reach the device again, depending on your settings."),
			addr  = (#keep > 0) and "192.168.1.1" or nil
		})
		fork_exec("sleep 1; killall dropbear uhttpd; sleep 1; /sbin/sysupgrade %s %q" %{ keep, image_tmp })
	end
end

function fork_exec(command)
	local pid = nixio.fork()
	if pid > 0 then
		return
	elseif pid == 0 then
		-- change to root dir
		nixio.chdir("/")

		-- patch stdin, out, err to /dev/null
		local null = nixio.open("/dev/null", "w+")
		if null then
			nixio.dup(null, nixio.stderr)
			nixio.dup(null, nixio.stdout)
			nixio.dup(null, nixio.stdin)
			if null:fileno() > 2 then
				null:close()
			end
		end

		-- replace with target command
		nixio.exec("/bin/sh", "-c", command)
	end
end

function action_logout()
	local dsp = require "luci.dispatcher"
	local utl = require "luci.util"
	local sid = dsp.context.authsession

	if sid then
		utl.ubus("session", "destroy", { ubus_rpc_session = sid })

		luci.http.header("Set-Cookie", "sysauth=%s; expires=%s; path=%s/" %{
				sid, 'Thu, 01 Jan 1970 01:00:00 GMT', dsp.build_url()
		})
	end

	luci.http.redirect(dsp.build_url())
end
