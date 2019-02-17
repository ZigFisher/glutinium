--[[
LuCI - Lua Configuration Interface

Copyright 2008 Steven Barth <steven@midlink.org>
Copyright 2008 Jo-Philipp Wich <xm@leipzig.freifunk.net>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

$Id: serialoverip.lua 5118 2009-07-23 03:32:30Z jow $
]]--
module("luci.controller.serialoverip", package.seeall)

function index()
	local page = entry({"admin", "services", "serialoverip"}, cbi("serialoverip/serialoverip"), "Serial Over IP", 99)
	page.dependent = false
end
