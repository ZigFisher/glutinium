#!/bin/sh

#audio enable inner codec
audio_disable_mute()
{
	himm 0x200f0078 0x0
	himm 0x201A0400 0x8
	himm 0x201A0020 0x8
}

#audio disable inner codec
audio_enable_mute()
{
	himm 0x200f0078 0x0
	himm 0x201A0400 0x8
	himm 0x201A0020 0x0
}

# (normal mode)
ir_cut_enable()
{
	# pin_mux
	himm 0x200f0100 0x1		# GPIO8_0
	himm 0x200f0104 0x1		# GPIO8_1
	
	# dir
	himm 0x201C0400	0x3
	# data, GPIO8_0: 0, GPIO8_1: 1  (normal mode)
	himm 0x201c000c 0x2
	
	# back to original 
	sleep 1;
	himm 0x201c000c 0x0
}

# (ir mode)
ir_cut_disable()
{
	# pin_mux
	himm 0x200f0100 0x1		# GPIO8_0
	himm 0x200f0104 0x1		# GPIO8_1
	
	# dir
	himm 0x201c0400	0x3
	# data, GPIO8_0: 1, GPIO8_1: 0  (ir mode)
	himm 0x201c000c 0x1
	
	# back to original 
	sleep 1
	himm 0x201c000c 0x0
}

isp_ai_piris_pin_mux()
{
    himm 0x200F0050 0x0;                 #P-Iris
    himm 0x200F0054 0x0;                 #P-Iris
    himm 0x200F0058 0x0;                 #P-Iris
    himm 0x200F005C 0x0;                 #P-Iris
}

isp_ai_pin_mux()
{
    # PWM1
    himm 0x200F00EC 0x0;
}


isp_ai_pwm_config()
{
    # 50MHz
    himm 0x20030038 0x6;
}

if [ $# -eq 0 ]; then
    echo "normal mode, mute off: ./demo_config.sh 0 0";
    echo "ir mode    , mute on : ./demo_config.sh 1 1";
else
    if [ $1 -eq 0 ]; then
        echo "normal mode, ir_cut on"
        ir_cut_enable > /dev/null;
    fi

    if [ $1 -eq 1 ]; then
        echo "ir mode, ir_cut off"
        ir_cut_disable > /dev/null ;
    fi
	
	if [ $2 -eq 0 ]; then
        echo "enable inner codec, mute off"
        audio_disable_mute > /dev/null;
    fi
	
	if [ $2 -eq 1 ]; then
        echo "disable inner codec, mute on"
        audio_enable_mute > /dev/null;
    fi
fi

#isp_ai_pin_mux;
#isp_ai_pwm_config;
#isp_ai_piris_pin_mux();
