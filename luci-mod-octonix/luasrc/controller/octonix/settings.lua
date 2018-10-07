-- Octonix Project (GPLv3) http://octonix.net

module("luci.controller.octonix.settings", package.seeall)
require("uci")

function index()
	entry({"octonix", "network"}, cbi("octonix/network", {autoapply=true}), _("Network"), 21)
end
