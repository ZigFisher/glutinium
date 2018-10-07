-- OpenWISP Project (GPLv3) http://openwisp.org

module("luci.controller.openwisp.settings", package.seeall)
require("uci")

function index()
	entry({"openwisp", "network"}, cbi("openwisp/network", {autoapply=true}), _("Network"), 21)
end
