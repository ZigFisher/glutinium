# openwrt-zabbixd
Ucified Zabbix Packages
You can use this feed to compile ucified zabbix_agentd packages.

## How to install
Add this repo in to your openwrt/lede feeds:
```
cp feeds.conf.default feeds.conf
echo src-git zabbix https://github.com/openwisp/openwrt-zabbixd.git >>feeds.conf
./scripts/feeds update -a
./scripts/feeds install -a
make menuconfig
```
enable zabbixd-uci package and extra zabbix package

Now you can compile

```
make -j1 V=s
```
