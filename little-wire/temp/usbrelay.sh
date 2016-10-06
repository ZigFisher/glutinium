#!/bin/sh
# plik do obslugi przekaznikow przez GPIO
# usbrelay.sh polecenie [numer_przekaznika]
# polecenie, pierwszy argument:
# off - wylacza przekaznik
#   usbrelay.sh off 1
# on - wlacza przekaznik
#   usbrelay.sh on 1
# status - podaje status wszystkich przekaznikow w postaci numer_przekaznika:on lub numer_przekaznika:off, kazdy w osobnej linii
#   usbrelay.sh status

LED1="/sys/class/gpio/gpio2/value"
LED2="/sys/class/gpio/gpio18/value"
LED3="/sys/class/gpio/gpio7/value"

case "$1" in
    off)
        case $2 in
            1)
            echo "0" > $LED1
            ;;
            2)
            echo "0" > $LED2
            ;;
            3)
            echo "0" > $LED3
            ;;
        esac
        ;;
    on)
        case $2 in
            1)
            echo "1" > $LED1
            ;;
            2)
            echo "1" > $LED2
            ;;
            3)
            echo "1" > $LED3
            ;;
        esac
        ;;
    status)
        echo -n "1:"
        [ $(cat $LED1) -eq "0" ] && echo "off" || echo "on"
        echo -n "2:"
        [ $(cat $LED2) -eq "0" ] && echo "off" || echo "on"
        echo -n "3:"
        [ $(cat $LED3) -eq "0" ] && echo "off" || echo "on"
        ;;
    who)
        usbrelay status | awk '/Relay is/ {printf "1:%s\n", $3}'
        ;;
esac

exit 0
