m = Map("serialoverip", "Serial Over IP", "Setup Serial Over IP Configuration, reboot to apply")

s = m:section(TypedSection, "serial", "General")
s.anonymous = true
s.addremove = true

p1 = s:option(ListValue, "type", "Serial Over IP / Serial to Network")
p1:value("serialoverip", "serialoverip")
p1:value("ser2net", "ser2net")
p1.default = "serialoverip"

q3 = s:option(Value, "ip", "Host IP", "The unicast/multicast address to send/receive on.")
q3:depends("type","serialoverip")

q4 = s:option(Value, "ipser", "IP Address", "Leave blank to bind to all the ports on the machine.") 
q4:depends("type","ser2net")

s:option(Value, "port", "Port", "The port to send/receive on")

p4 = s:option(ListValue, "baud", "Baud Rate")
p4:value("50", "50")
p4:value("110", "110")
p4:value("300","300")
p4:value("1200", "1200")
p4:value("2400", "2400")
p4:value("4800", "4800")
p4:value("9600", "9600")
p4:value("19200", "19200")
p4:value("38400", "38400")
p4:value("57600", "57600")
p4:value("115200", "115200")
p4:value("230400", "230400")
p4.default = "115200"

p5 = s:option(ListValue, "device", "Device", "The serial port device")
p5:value("ttyS1", "ttyS1")
p5:value("ttyS2", "ttyS2")
p5:value("ttyS3", "ttyS3")
p5:value("ttyS4", "ttyS4")
p5:value("ttyS5", "ttyS5")
p5:value("ttyS6", "ttyS6")
p5:value("ttyS7", "ttyS7")
p5:value("ttyS8", "ttyS8")
p5:value("ttyS9", "ttyS9")
p5:value("ttyS10", "ttyS10")
p5:value("ttyS11", "ttyS11")
p5:value("ttyS12", "ttyS12")
p5:value("ttyS13", "ttyS13")
p5:value("ttyS14", "ttyS14")
p5:value("ttyS15", "ttyS15")
p5:value("ttyS16", "ttyS16")
p5:value("ttyS17", "ttyS17")
p5:value("tty$18", "ttyS18")
p5:value("ttyS19", "ttyS19")
p5:value("ttyS20", "ttyS20")
p5.default = "ttyS1"

p6 = s:option(ListValue, "data", "Data Bits")
p6:value("5", "5")
p6:value("6", "6")
p6:value("7", "7")
p6:value("8", "8")
p6.default = "8"

p7 = s:option(ListValue, "parity", "Parity Type")
p7:value("N", "N")
p7:value("E", "E")
p7:value("O", "O")
p7.default = "N"

p8 = s:option(ListValue, "stop", "Stop Bits")
p8:value("1", "1")
p8:value("2", "2")
p8.default = "1"

p9 = s:option(ListValue, "state", "State")
p9:value("raw","raw")
p9:value("rawlp","rawlp")
p9:value("telnet","telnet")
p9:value("off","off")
p9:depends("type","ser2net")
p9.default = "raw"

q1 = s:option(Value, "timeout", "Time Out", "Time before port will be disconnected if there is no activity (0 disables this function)")
q1:depends("type","ser2net")
q1.default = "0"

q2 = s:option(ListValue, "remctl", "Remote Control")
q2:value("ON","ON")
q2:value("OFF","OFF")
q2:depends("type","ser2net")
q2.default = "OFF"

return m
