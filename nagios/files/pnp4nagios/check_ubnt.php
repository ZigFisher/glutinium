<?php
#
# Ubiquiti pnp4nagios template
#
# By Zig Fisher
# http://zftlab.org
#
# Colors helping table: http://html-color-codes.info/Cvetovye-kody-HTML/

$_C_WARNRULE  = '#FFFF00';
$_C_CRITRULE  = '#FF0000';
$_C_LINE      = '#000000';
$_C_SIGNAL    = '#04B4AE';
$_C_NOISE     = '#B40431';
$_C_CCQ       = '#DF7401';
$_C_RXRATE    = '#00FF40';
$_C_TXRATE    = '#2E64FE';
$_C_RXDATA    = '#00FF00';
$_C_TXDATA    = '#2E64FE';
$_C_AVERAGE   = '#FF0000';
$_C_CONN      = '#642EFE';
$_C_MEMORY    = '#642EFE';
$_C_UPTIME    = '#DF7001';
$_C_AQUALITY  = '#ff00ff';
$_C_ACAPACITY = '#000099';

$_RXDATA    = $this->DS[0];
$_TXDATA    = $this->DS[1];
$_SIGNAL    = $this->DS[2];
$_NOISE     = $this->DS[3];
$_RXRATE    = $this->DS[4];
$_TXRATE    = $this->DS[5];
$_CCQ       = $this->DS[6];
$_CONN      = $this->DS[7];
$_LAVG      = $this->DS[8];
$_MEMORY    = $this->DS[9];
#$_UPTIME    = $this->DS[10];

$_SIGMIN = min ($_SIGNAL['MIN'], $_NOISE['MIN']);
$_SIGMAX = max ($_SIGNAL['MAX'], $_NOISE['MAX']);

$ds_name[0] = "Network Interface Traffic";
$opt[0] = "--vertical-label 'traffic, bps' -b 1024 --title '{$this->MACRO['DISP_HOSTNAME']}' --lower-limit=0 --slope-mode --watermark 'Special edition for Kentok' ";
$def[0]  = "DEF:rxdata={$_RXDATA['RRDFILE']}:{$_RXDATA['DS']}:AVERAGE ";
$def[0] .= "DEF:txdata={$_TXDATA['RRDFILE']}:{$_TXDATA['DS']}:AVERAGE ";
$def[0] .= "CDEF:ibits=rxdata,8,* ";
$def[0] .= "CDEF:obits=txdata,8,* ";
#$def[0] .= "AREA:ibits{$_C_RXDATA}:'in  ' ";
$def[0] .= "LINE2:ibits{$_C_RXDATA}:'in  ' ";
$def[0] .= "GPRINT:ibits:LAST:'%7.2lf %S last' ";
$def[0] .= "GPRINT:ibits:AVERAGE:'%7.2lf %S avg' ";
$def[0] .= "GPRINT:ibits:MAX:'%7.2lf %S max\\n' ";
#$def[0] .= "AREA:obits{$_C_TXDATA}:'out ' " ;
$def[0] .= "LINE2:obits{$_C_TXDATA}:'out ' " ;
$def[0] .= "GPRINT:obits:LAST:'%7.2lf %S last' " ;
$def[0] .= "GPRINT:obits:AVERAGE:'%7.2lf %S avg' " ;
$def[0] .= "GPRINT:obits:MAX:'%7.2lf %S max\\n' ";

$ds_name[1] = "Signal & Noise";
$opt[1] = "--vertical-label 'signal/noise, dBm' --title '{$this->MACRO['DISP_HOSTNAME']}' --alt-y-grid --slope-mode --watermark 'Special edition for Kentok' ";
$def[1]  = "DEF:signal={$_SIGNAL['RRDFILE']}:{$_SIGNAL['DS']}:AVERAGE ";
$def[1] .= "DEF:noise={$_NOISE['RRDFILE']}:{$_NOISE['DS']}:AVERAGE ";
$def[1] .= "CDEF:signalU=signal,0,EQ,NEGINF,signal,IF ";
$def[1] .= "CDEF:noiseU=noise,0,EQ,UNKN,noise,IF ";
$def[1] .= "CDEF:signalI=signalU,UN,UNKN,NEGINF,IF ";
$def[1] .= "CDEF:noiseI=noiseU,UN,UNKN,NEGINF,IF ";
$def[1] .= "LINE1:signalU{$_C_SIGNAL}:'signal' ";
$def[1] .= "AREA:signalI{$_C_SIGNAL}:'':STACK ";
$def[1] .= "GPRINT:signalU:LAST:'%7.2lf last' ";
$def[1] .= "GPRINT:signalU:AVERAGE:'%7.2lf avg' ";
$def[1] .= "GPRINT:signalU:MAX:'%7.2lf max' ";
$def[1] .= "GPRINT:signalU:MIN:'%7.2lf min'\\n ";
$def[1] .= "LINE1:noiseU{$_C_NOISE}:'noise ' ";
$def[1] .= "AREA:noiseI{$_C_NOISE}:'':STACK ";
$def[1] .= "GPRINT:noiseU:LAST:'%7.2lf last' ";
$def[1] .= "GPRINT:noiseU:AVERAGE:'%7.2lf avg' ";
$def[1] .= "GPRINT:noiseU:MAX:'%7.2lf max' ";
$def[1] .= "GPRINT:noiseU:MIN:'%7.2lf min'\\n ";
$def[1] .= "LINE2:signalU{$_C_LINE}:'' ";
$def[1] .= "LINE2:noiseU{$_C_LINE} ";

if($this->MACRO['TIMET'] != ""){
  $def[1] .= "VRULE:".$this->MACRO['TIMET']."#000000:\"Last Service Check \\n\" ";
}
if ($WARN[1] != "") {
  $def[1] .= "HRULE:$WARN[1]{$_C_WARNRULE}:\"In-Traffic Warning on $WARN[1] \" ";
}
if ($CRIT[1] != "") {
  $def[1] .= "HRULE:$CRIT[1]{$_C_CRITRULE}:\"In-Traffic Critical on $CRIT[1] \" ";
}

$ds_name[2] = "Connect rate";
$opt[2] = "--vertical-label 'rate, Mbit/s' --title '{$this->MACRO['DISP_HOSTNAME']}' --lower-limit=0 --slope-mode --watermark 'Special edition for Kentok' ";
$def[2] = "DEF:rxrate={$_RXRATE['RRDFILE']}:{$_RXRATE['DS']}:AVERAGE ";
$def[2] .= "DEF:txrate={$_TXRATE['RRDFILE']}:{$_TXRATE['DS']}:AVERAGE ";
$def[2] .= "LINE2:rxrate{$_C_RXRATE}:'rx' ";
$def[2] .= "GPRINT:rxrate:LAST:'%7.2lf last' ";
$def[2] .= "GPRINT:rxrate:AVERAGE:'%7.2lf avg' ";
$def[2] .= "GPRINT:rxrate:MAX:'%7.2lf max' ";
$def[2] .= "GPRINT:rxrate:MIN:'%7.2lf min'\\n ";
$def[2] .= "LINE2:txrate{$_C_TXRATE}:'tx' ";
$def[2] .= "GPRINT:txrate:LAST:'%7.2lf last' ";
$def[2] .= "GPRINT:txrate:AVERAGE:'%7.2lf avg' ";
$def[2] .= "GPRINT:txrate:MAX:'%7.2lf max' ";
$def[2] .= "GPRINT:txrate:MIN:'%7.2lf min'\\n ";

$ds_name[3] = "Link quiality";
$opt[3] = "--vertical-label 'quality, %' --title '{$this->MACRO['DISP_HOSTNAME']}' --lower-limit=0 --slope-mode --watermark 'Special edition for Kentok' ";
$def[3] = "DEF:ccq={$_CCQ['RRDFILE']}:{$_CCQ['DS']}:AVERAGE ";
$def[3] .= "AREA:ccq{$_C_CCQ}:'ccq' ";
$def[3] .= "GPRINT:ccq:LAST:'%7.2lf last' ";
$def[3] .= "GPRINT:ccq:AVERAGE:'%7.2lf avg' ";
$def[3] .= "GPRINT:ccq:MAX:'%7.2lf max' ";
$def[3] .= "GPRINT:ccq:MIN:'%7.2lf min'\\n ";

$ds_name[4] = "Connections";
$opt[4] = "--vertical-label 'people, ps' --title '{$this->MACRO['DISP_HOSTNAME']}' --lower-limit=0 --slope-mode --watermark 'Special edition for Kentok' ";
$def[4] = "DEF:conn={$_CONN['RRDFILE']}:{$_CONN['DS']}:AVERAGE ";
$def[4] .= "AREA:conn{$_C_CONN}:'conn' ";
$def[4] .= "GPRINT:conn:LAST:'%7.2lf %S last' ";
$def[4] .= "GPRINT:conn:AVERAGE:'%7.2lf %S avg' ";
$def[4] .= "GPRINT:conn:MAX:'%7.2lf max' ";
$def[4] .= "GPRINT:conn:MIN:'%7.2lf min'\\n ";

$ds_name[5] = "Load average";
$opt[5] = "--vertical-label 'usage system' --title '{$this->MACRO['DISP_HOSTNAME']}' --lower-limit=0 --slope-mode --watermark 'Special edition for Kentok' ";
$def[5] = "DEF:lavg={$_LAVG['RRDFILE']}:{$_LAVG['DS']}:AVERAGE ";
$def[5] .= "AREA:lavg{$_C_AVERAGE}:'load' ";
$def[5] .= "GPRINT:lavg:LAST:'%7.2lf last' ";
$def[5] .= "GPRINT:lavg:AVERAGE:'%7.2lf avg' ";
$def[5] .= "GPRINT:lavg:MAX:'%7.2lf max' ";
$def[5] .= "GPRINT:lavg:MIN:'%7.2lf min'\\n ";

$ds_name[6] = "Free Memory";
$opt[6] = "--vertical-label 'memory, Bytes' --title '{$this->MACRO['DISP_HOSTNAME']}' --lower-limit=0 --slope-mode --watermark 'Special edition for Kentok' ";
$def[6] = "DEF:mem={$_MEMORY['RRDFILE']}:{$_MEMORY['DS']}:AVERAGE ";
$def[6] .= "AREA:mem{$_C_MEMORY}:'mem' ";
$def[6] .= "GPRINT:mem:LAST:'%7.2lf last' ";
$def[6] .= "GPRINT:mem:AVERAGE:'%7.2lf avg' ";
$def[6] .= "GPRINT:mem:MAX:'%7.2lf max' ";
$def[6] .= "GPRINT:mem:MIN:'%7.2lf min'\\n ";

#$ds_name[7] = "Uptime";
#$opt[7] = "--vertical-label 'uptime, days' --title '{$this->MACRO['DISP_HOSTNAME']}' --lower-limit=0 --slope-mode --watermark 'Special edition for Kentok' ";
#$def[7] = "DEF:uptime={$_UPTIME['RRDFILE']}:{$_UPTIME['DS']}:AVERAGE ";
#$def[7] .= "CDEF:days=uptime,86400,/ ";
#$def[7] .= "AREA:days{$_C_UPTIME}:'day' ";
#$def[7] .= "GPRINT:days:LAST:'%7.2lf last' ";
#$def[7] .= "GPRINT:days:AVERAGE:'%7.2lf avg' ";
#$def[7] .= "GPRINT:days:MAX:'%7.2lf max'\\n ";

?>
