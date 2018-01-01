#!/bin/sh

HOST_1="hydra"
HOST_2="mutalisk"
PASS="YoYo"
INFO=".1.3.6.1.2.1.25.2.3.1.6.65536"
TITL="Mikrotik NAS memory"
NAME="Bytes"
DATA="/var/www/nas/micromon_mikrotik_nas_mem.rrd"
GRAF_D="/var/www/nas/micromon_mikrotik_nas_mem_d.png"
GRAF_W="/var/www/nas/micromon_mikrotik_nas_mem_w.png"


# Если база не существует - создаём её
if [ ! -f ${DATA} ]; then
    rrdtool create ${DATA} \
        DS:${HOST_1}:GAUGE:600:U:U \
        DS:${HOST_2}:GAUGE:600:U:U \
        RRA:AVERAGE:0.9:1:8640
fi

# Опрашиваем устройства
SNMP_1=`snmpwalk -Oqsv -v 2c -c ${PASS} ${HOST_1} ${INFO}`
SNMP_2=`snmpwalk -Oqsv -v 2c -c ${PASS} ${HOST_2} ${INFO}`

# Записываем информацию в базу
rrdtool update ${DATA} N:${SNMP_1}:${SNMP_2}

# Рисуем график дневной
rrdtool graph ${GRAF_D} -a PNG --start end-1d --slope-mode \
    --vertical-label ${NAME} --watermark "Special edition for Kentok" --title "${TITL}" \
    DEF:${HOST_1}=${DATA}:${HOST_1}:AVERAGE \
    LINE2:${HOST_1}#000044:"${HOST_1}   " \
    GPRINT:${HOST_1}:LAST:" Current\:%8.2lf %s"  \
    GPRINT:${HOST_1}:AVERAGE:"Average\:%8.2lf %s"  \
    GPRINT:${HOST_1}:MAX:"Maximum\:%8.2lf %s\n"  \
    DEF:${HOST_2}=${DATA}:${HOST_2}:AVERAGE \
    LINE2:${HOST_2}#FF5555:"${HOST_2}" \
    GPRINT:${HOST_2}:LAST:" Current\:%8.2lf %s"  \
    GPRINT:${HOST_2}:AVERAGE:"Average\:%8.2lf %s"  \
    GPRINT:${HOST_2}:MAX:"Maximum\:%8.2lf %s\n"  \
    -w 500 -h 180 > /dev/null

# Рисуем график недельный
rrdtool graph ${GRAF_W} -a PNG --start end-1w --slope-mode \
    --vertical-label ${NAME} --watermark "Special edition for Kentok" --title "${TITL}" \
    DEF:${HOST_1}=${DATA}:${HOST_1}:AVERAGE \
    LINE2:${HOST_1}#000044:"${HOST_1}   " \
    GPRINT:${HOST_1}:LAST:" Current\:%8.2lf %s"  \
    GPRINT:${HOST_1}:AVERAGE:"Average\:%8.2lf %s"  \
    GPRINT:${HOST_1}:MAX:"Maximum\:%8.2lf %s\n"  \
    DEF:${HOST_2}=${DATA}:${HOST_2}:AVERAGE \
    LINE2:${HOST_2}#FF5555:"${HOST_2}" \
    GPRINT:${HOST_2}:LAST:" Current\:%8.2lf %s"  \
    GPRINT:${HOST_2}:AVERAGE:"Average\:%8.2lf %s"  \
    GPRINT:${HOST_2}:MAX:"Maximum\:%8.2lf %s\n"  \
    -w 500 -h 180 > /dev/null

#    AREA:${HOST_1}#000099 \
#    AREA:${HOST_2}#009900 \
