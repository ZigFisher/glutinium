
STAGING_DIR=/var/www/builder/data/trunk/build_ng/openwrt/staging_dir
export STAGING_DIR
CFLAGS=-I.
#
# Atheros
CC= ~/trunk/build_ng/openwrt/staging_dir/toolchain-mips_r2_gcc-4.6-linaro_uClibc-0.9.33.2/bin/mips-openwrt-linux-uclibc-gcc
#
# Broadcom
#CC= ~/trunk/build_ng/openwrt/staging_dir/toolchain-mips_gcc-4.6-linaro_uClibc-0.9.33.2/bin/mips-openwrt-linux-uclibc-gcc



ina219:
	$(CC) -o ina219 ina219.c

bmp085:
	$(CC) -o bmp085 bmp085.c smbus.c

lm75:
	$(CC) -o lm75 lm75.c

ds1621:
	$(CC) -o ds1621 ds1621.c

bh1750:
	$(CC) -o bh1750 bh1750.c smbus.c

lcdd:
	$(CC) -o lcdd lcdd.c

mcp_rw:
	$(CC) -o mcp mcp_rw.c smbus.c
