local os     = require "os";
local fs     = require "nixio.fs";
local config = 'ipcam';

local m, d, a, s, dm, t;

if fs.stat('/etc/config/'..config, "type") ~= "reg" then
    os.execute('touch /etc/config/'..config)
end

m = Map(config, translate("SD Card Settings"), translate("Here you can configure sd card settings."));

s = "sdcard";

dm = m:section(NamedSection, "sdcard", "");

t = dm:option(DummyValue, "_capacity", translate("Capacity"))
t.template = "admin_ipcam/sdcard_status"

a = dm:option(Flag, "photo", "Save event photo to SD card");
a.optional = false; a.rmempty = false;

a = dm:option(Flag, "video", "Save event video to SD card");
a.optional = false; a.rmempty = false;

a = dm:option(Flag, "overwrite", "Over-write when full");
a.optional = false; a.rmempty = false;

t = dm:option(DummyValue, "_format_sd_card_f2fs", "Format SD Card")
t.template = "admin_ipcam/sdcard_format_f2fs"

t = dm:option(DummyValue, "_format_sd_card_vfat", "Format SD Card")
t.template = "admin_ipcam/sdcard_format_vfat"

return m
