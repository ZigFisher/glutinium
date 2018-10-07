-- Copyright 2008 Steven Barth <steven@midlink.org>
-- Copyright 2008 Jo-Philipp Wich <jow@openwrt.org>
-- Re-adapted for the OpenWISP Project (GPLv3) http://openwisp.org

m = Map("network", translate("Setup"))

s = m:section(NamedSection, "lan", "interface", translate("LAN interface"))
s.addremove = false

p = s:option(ListValue, "proto", translate("Protocol"))
p.override_values = true
p:value("static", translate("Static IP"))
p:value("dhcp", translate("DHCP client"))

ip = s:option(Value, "ipaddr", translate("<abbr title=\"Internet Protocol Version 4\">IPv4</abbr>-Address"))
ip:depends("proto", "static")

nm = s:option(Value, "netmask", translate("<abbr title=\"Internet Protocol Version 4\">IPv4</abbr>-Netmask"))
nm:depends("proto", "static")
nm:value("255.255.255.0")
nm:value("255.255.0.0")
nm:value("255.0.0.0")

gw = s:option(Value, "gateway", translate("<abbr title=\"Internet Protocol Version 4\">IPv4</abbr>-Gateway") .. translate(" (optional)"))
gw:depends("proto", "static")
gw.rmempty = true

dns = s:option(Value, "dns", translate("<abbr title=\"Domain Name System\">DNS</abbr>-Server") .. translate(" (optional)"))
dns:depends("proto", "static")
dns.rmempty = true

return m
