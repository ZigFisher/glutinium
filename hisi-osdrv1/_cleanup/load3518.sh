#!/bin/sh
#
# Useage: ./load3518 [ -r|-i|-a ] [ sensor ]
#         -r : rmmod all modules
#         -i : insmod all modules
#    default : rmmod all moules and then insmod them
#
#
# 9m034 ar0130 ar0140 ar0141 ar0330 ar0331 bf3116 bg0703 gc1004 gc1014 hm1375 icx692 imx104 imx122 imx138 imx225 imx236 mn34031 mn34041 mt9p006 ov2710 ov9712 po3100k soih22


WORK_DIR=/etc/hi3518xv100

SNS_A=ar0130

if [ $# -ge 2 ]; then
    SNS_A=$2
fi

report_error()
{
    echo "******* Error: There's something wrong, please check! *****"
    exit 1
}

insert_audio()
{
   insmod acodec.ko
   insmod hidmac.ko
   insmod hi3518_sio.ko
   insmod hi3518_ai.ko
   insmod hi3518_ao.ko
   insmod hi3518_aenc.ko
   insmod hi3518_adec.ko
   echo "Insert audio modules"
}

remove_audio()
{
    rmmod hi3518_adec
    rmmod hi3518_aenc
    rmmod hi3518_ao
    rmmod hi3518_ai
    rmmod hi3518_sio
    rmmod acodec
    rmmod hidmac
    echo "Remove audio modules"
}

insert_sns()
{
    case $SNS_A in

        ar0130|9m034|po3100k|bf3116|bg0703)
            devmem  0x20030030 32 0x5;          # Sensor clock 27 MHz
            insmod  ssp_ad9020.ko
            ;;

        ar0140|ar0141)
            himm 0x20030030 0x1                 # Sensor clock 24 MHz
            ;;

        icx692)
            devmem  0x200f000c 32 0x1;          # pinmux SPI0
            devmem  0x200f0010 32 0x1;          # pinmux SPI0
            devmem  0x200f0014 32 0x1;          # pinmux SPI0
            insmod  ssp_ad9020.ko
            ;;

        mn34031|mn34041)
            devmem  0x200f000c 32 0x1;          # pinmux SPI0
            devmem  0x200f0010 32 0x1;          # pinmux SPI0
            devmem  0x200f0014 32 0x1;          # pinmux SPI0
            devmem  0x20030030 32 0x5;          # Sensor clock 27MHz
            insmod  ssp_pana.ko
            ;;


        imx104|imx122|imx138|imx225)
            devmem  0x200f000c 32 0x1;          # pinmux SPI0
            devmem  0x200f0010 32 0x1;          # pinmux SPI0
            devmem  0x200f0014 32 0x1;          # pinmux SPI0
            devmem  0x20030030 32 0x6;          # Sensor clock 37.125 MHz
            insmod  ssp_sony.ko
            ;;

        ov9712|soih22|ov2710)
            devmem  0x20030030 32 0x1;          # Sensor clock 24 MHz
            insmod  ssp_ad9020.ko
            ;;

        mt9p006)
            devmem  0x20030030 32 0x1;          # Sensor clock 24 MHz
            devmem  0x2003002c 32 0x6a;         # VI input associated clock phase reversed
            insmod  ssp_ad9020.ko
            ;;

        hm1375|ar0330|gc1004|gc1014)
            devmem  0x20030030 32 0x1           # Sensor clock 24 MHz
            ;;

        ar0331)
            devmem  0x20030030 32 0x5           # Sensor clock 27MHz
            ;;

        imx236)
            devmem  0x20030030 32 0x6;          # Sensor clock 37.125 MHz
            ;;

        *)
            echo "Invalid sensor type $SNS_A xxxx"
            report_error;;
    esac
}

remove_sns()
{
    rmmod hi_i2c
    rmmod ssp
    rmmod ssp_sony
    rmmod ssp_pana
    rmmod ssp_ad9020
}

mmz_mem_info()
{
    BASEADDR=$((0x80000000))
    TOTALMEM=$((0x04000000))
    local osmem=$(($(cat /proc/cmdline 2>/dev/null | \
                  sed -nr 's/mem=([^[:space:]]+).*/\1/p' | \
                  sed -r  's/[kK]$/\*1024/;s/[mM]$/\*1024\*1024/')))
    local maddr=$(printf "0x%x" $((BASEADDR+osmem)))
    while [ $osmem -ge $TOTALMEM ]; do TOTALMEM=$((TOTALMEM<<1)); done
    local msize=$(printf "0x%x" $((TOTALMEM-osmem)))
    #
    printf 'anonymous,0,0x%x,0x%x' ${maddr} ${msize}
}

insert_ko()
{
    # Set low power control                                                    #
    #source ${WORK_DIR}/lowpower.sh                                            #
    #                                                                          #
    # Set pinmux configuration                                                 #
    source ${WORK_DIR}/pinmux_hi3518.sh rmii i2c                               #
    #                                                                          #
    # Set clock configuration                                                  #
    #source ${WORK_DIR}/clkcfg_hi3518.sh                                       #
    #                                                                          #
    # Hisilicon drivers loading                                                #
    insmod mmz.ko mmz=$(mmz_mem_info) anony=1 || report_error                  # Load OK
    #                                                                          #
    #insmod mmz.ko mmz=anonymous,0,0x82b00000,0x1500000 anony=1                # Zft Lab. test hand made
    #insmod mmz.ko mmz=anonymous,0,0x86000000,64M anony=1 || report_error      # From original for HI3518
    #insmod mmz.ko mmz=anonymous,0,0x82000000,32M anony=1 || report_error      # From original for HI3518(E) only
    #                                                                          #
    insmod hi3518_base.ko                                                      # Load OK
    insmod hi3518_sys.ko                                                       # Load OK
    insmod hiuser.ko                                                           # Load OK
    insmod hi3518_tde.ko                                                       # Load OK
    insmod hi3518_dsu.ko                                                       # Load OK
    insmod hi3518_viu.ko ext_csc_en=0 csc_ct_mode=1 csc_tv_en=1                # Load OK
    insmod hi3518_isp.ko                                                       # Load OK  for hi3518(E) only ?
    insmod hi3518_vpss.ko                                                      # Load OK
    #insmod hi3518_vou.ko                                                      # Load OK  not used in buildroot
    #insmod hi3518_vou.ko detectCycle=0 #close dac detect                      # Load OK
    #insmod hifb.ko video="hifb:vram0_size:1620"                               # Load OK  not used in buildroot
    insmod hi3518_venc.ko                                                      # Load OK
    insmod hi3518_group.ko                                                     # Load OK
    insmod hi3518_chnl.ko                                                      # Load OK
    insmod hi3518_h264e.ko                                                     # Load OK
    insmod hi3518_jpege.ko                                                     # Load OK
    insmod hi3518_rc.ko                                                        # Load OK
    insmod hi3518_region.ko                                                    # Load OK
    insmod hi3518_vda.ko                                                       # Load OK
    insmod hi3518_ive.ko                                                       # Load OK
    #                                                                          #
    # External drivers loading                                                 #
    insmod hi_i2c.ko                                                           # Load OK
    #insmod gpioi2c.ko                                                         #
    #insmod gpioi2c_ex.ko                                                      #
    insmod pwm.ko                                                              # Load OK
    #insmod sil9024.ko norm=5  #720P@60fps                                     #
    #                                                                          #
    insert_sns                                                                 # Load group OK
    #insmod hi3518_isp.ko                                                      #    secondary ?
    insert_audio                                                               # Load group OK
    #                                                                          #
    echo "==== Your input Sensor type is $SNS_A ===="                          #
    #                                                                          #
    # Set system configuration                                                 #
    #source ${WORK_DIR}/sysctl_hi3518.sh                                       #
}

remove_ko()
{
    remove_audio
    remove_sns
    #
    rmmod sil9024
    rmmod hi_i2c.ko
    rmmod pwm
    #rmmod gpioi2c
    #
    rmmod hi3518_ive
    rmmod hi3518_vda
    rmmod hi3518_region
    rmmod hi3518_rc
    rmmod hi3518_jpege
    rmmod hi3518_h264e
    rmmod hi3518_chnl
    rmmod hi3518_group
    rmmod hi3518_venc
    rmmod hifb
    rmmod hi3518_vou
    rmmod hi3518_vpss
    rmmod hi3518_isp
    rmmod hi3518_viu
    rmmod hi3518_dsu
    rmmod hi3518_tde
    rmmod hiuser
    rmmod hi3518_sys
    rmmod hi3518_base
    rmmod mmz
}

load_usage()
{
    echo "Usage:  ./load3518 [-option] [sensor_name]"
    echo "options:"
    echo "    -i sensor_name           insert modules"
    echo "    -r                       remove modules"
    echo "    -a sensor_name           remove modules first, then insert modules"
    echo "    -h                       help information"
    echo -e "Available sensors: 9m034 ar0130 ar0140 ar0141 ar0330 ar0331 bf3116 bg0703 gc1004 gc1014 hm1375 icx692 imx104 imx122 imx138 imx225 imx236 mn34031 mn34041 mt9p006 ov2710 ov9712 po3100k soih22"
    echo -e "for example: ./load3518 -a ar0130 \n"
}

# load module.
if [ "$1" = "-i" ]
then
    insert_ko
fi

if [ "$1" = "-r" ]
then
    remove_ko
fi

if [ "$1" = "-h" ]
then
    load_usage
    exit
fi

if [ $# -eq 0 ] || [ "$1" = "-a" ]
then
    remove_ko
    echo "#########################################################################"
    insert_ko
fi
