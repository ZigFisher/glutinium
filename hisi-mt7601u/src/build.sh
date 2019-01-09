KERNEL_DIR=$(pwd)/../../output/build/linux-3.4.y/
make clean
make LINUX_SRC=$KERNEL_DIR PLATFORM=HI3518C CHIPSET=7601U -j1 V=99
