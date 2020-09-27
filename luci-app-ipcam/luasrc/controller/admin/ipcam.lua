module("luci.controller.admin.ipcam", package.seeall)

util   = require "luci.util"


function index()

        entry({"admin", "ipcam"}, alias("admin", "ipcam", "settings"), _("IPCam"), 55).index = true

        entry({"admin", "ipcam", "sdcard_settings"}, cbi("admin_ipcam/sdcard_settings"), _("SD Card Settings"), 11)

        entry({"admin", "ipcam", "sdcard_status"}, call("action_sdcard_status"))

        entry({"admin", "ipcam", "sdcard_format_f2fs"}, call("action_sdcard_format_f2fs"))

        entry({"admin", "ipcam", "sdcard_format_vfat"}, call("action_sdcard_format_vfat"))

        entry({"admin", "ipcam", "test_mail"}, call("action_send_mail"))

end


function sdcard_present()
	return (os.execute("mount |grep mmcblk0") == 0)
end


function action_sdcard_status()
        local sd_status = "detecting..."
        if( sdcard_present() == true ) then
                sd_status = util.exec("df -h /dev/mmcblk0|awk '/mmc/ {print $4}'")
        end
        luci.http.prepare_content("application/json")
        luci.http.write_json({ sdstring = sd_status })

end


function action_sdcard_format_f2fs()
        if( sdcard_present() == true ) then
                os.execute("echo dd if=/dev/zero of=/dev/mmcblk0 bs=4M count=1 ; mkfs.f2fs -l IPCam /dev/mmcblk0")
                luci.http.status(200, "Formated")
        end
end

function action_sdcard_format_vfat()
        if( sdcard_present() == true ) then
                os.execute("echo dd if=/dev/zero of=/dev/mmcblk0 bs=4M count=1 ; mkfs.vfat -v -n IPCam /dev/mmcblk0")
                luci.http.status(200, "Formated")
        end
end


function create_temp_file(from, rcpt, subject)
	local jiji = os.tmpname()
	local f = io.open(jiji, "w+")

	f:write("From: "..from.."\nTo: "..rcpt.."\nSubject: "..subject.."\n\n Hello from IPCam!\n")
	f:close()
	return jiji
end


function action_send_mail()
        local server = (luci.http.formvalue("server"))
	local sender = (luci.http.formvalue("sender"))
	local password = (luci.http.formvalue("password"))
	local rcpt = (luci.http.formvalue("rcpt"))
        local subject = (luci.http.formvalue("subject"))
        local port = tonumber(luci.http.formvalue("port"))
        local auth = (luci.http.formvalue("auth"))

	temp_file = create_temp_file(sender, rcpt, subject)
	if( auth == "true") then
		os.execute("curl  --url \'"..server..":"..port.."\' --ssl-reqd --mail-from \'"..sender.."\' --mail-rcpt \'"..rcpt.."\' --upload-file "..temp_file.." --user \'"..sender..":"..password.."\' --insecure");
	else
		os.execute("curl  --url \'"..server..":"..port.."\' --ssl-reqd --mail-from \'"..sender.."\' --mail-rcpt \'"..rcpt.."\' --upload-file "..temp_file.." --insecure");
	end
	os.execute("rm "..temp_file.."")

end
