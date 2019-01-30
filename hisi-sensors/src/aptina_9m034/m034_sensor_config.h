
//9m034 720P 30fps linear
static int sensor_rom_30_lin[] = {
        // [Linear Mode 720p 30fps AE=on EMBEDDED=on]
//[Demo Initialization]
//Load = Reset
//[Reset]
        0xFFFE00C8, // wait 8 frames
        0x301A10D9,
		0xFFFE0005, // delay 5ms after sensor reset
        0x301A10D8,
//[Reset]//
        0xFFFE00C8,
//Load = A-1000ERS Rev3 Hidy and linear sequencer load December 16 2010
//[A-1000ERS Rev3 Hidy and linear sequencer load December 16 2010]
//HiDY sequencer CR 31644
        0x30888000, //SEQ_CTRL_PORT
        0x30860025, //SEQ_DATA_PORT
        0x30865050, //SEQ_DATA_PORT
        0x30862D26, //SEQ_DATA_PORT
        0x30860828, //SEQ_DATA_PORT
        0x30860D17, //SEQ_DATA_PORT
        0x30860926, //SEQ_DATA_PORT
        0x30860028, //SEQ_DATA_PORT
        0x30860526, //SEQ_DATA_PORT
        0x3086A728, //SEQ_DATA_PORT
        0x30860725, //SEQ_DATA_PORT
        0x30868080, //SEQ_DATA_PORT
        0x30862925, //SEQ_DATA_PORT
        0x30860040, //SEQ_DATA_PORT
        0x30862702, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30862706, //SEQ_DATA_PORT
		//0x30888020, //write 0x3088 another time

        0x30861F17, //SEQ_DATA_PORT
        0x30863626, //SEQ_DATA_PORT
        0x3086A617, //SEQ_DATA_PORT
        0x30860326, //SEQ_DATA_PORT
        0x3086A417, //SEQ_DATA_PORT
        0x30861F28, //SEQ_DATA_PORT
        0x30860526, //SEQ_DATA_PORT
        0x30862028, //SEQ_DATA_PORT
        0x30860425, //SEQ_DATA_PORT
        0x30862020, //SEQ_DATA_PORT
        0x30862700, //SEQ_DATA_PORT
        0x3086171D, //SEQ_DATA_PORT
        0x30862500, //SEQ_DATA_PORT
        0x30862017, //SEQ_DATA_PORT
        0x30861028, //SEQ_DATA_PORT
        0x30860519, //SEQ_DATA_PORT
        0x30861703, //SEQ_DATA_PORT
        0x30862706, //SEQ_DATA_PORT
        0x30861703, //SEQ_DATA_PORT
        0x30861741, //SEQ_DATA_PORT
        0x30862660, //SEQ_DATA_PORT
        0x3086175A, //SEQ_DATA_PORT
        0x30862317, //SEQ_DATA_PORT
        0x30861122, //SEQ_DATA_PORT
        0x30861741, //SEQ_DATA_PORT
        0x30862500, //SEQ_DATA_PORT
        0x30869027, //SEQ_DATA_PORT
        0x30860026, //SEQ_DATA_PORT
        0x30861828, //SEQ_DATA_PORT
        0x3086002E, //SEQ_DATA_PORT
        0x30862A28, //SEQ_DATA_PORT
        0x3086081C, //SEQ_DATA_PORT
        0x30861470, //SEQ_DATA_PORT
        0x30867003, //SEQ_DATA_PORT
        0x30861470, //SEQ_DATA_PORT
        0x30867004, //SEQ_DATA_PORT
        0x30861470, //SEQ_DATA_PORT
        0x30867005, //SEQ_DATA_PORT
        0x30861470, //SEQ_DATA_PORT
        0x30867009, //SEQ_DATA_PORT
        0x3086170C, //SEQ_DATA_PORT
        0x30860014, //SEQ_DATA_PORT
        0x30860020, //SEQ_DATA_PORT
        0x30860014, //SEQ_DATA_PORT
        0x30860050, //SEQ_DATA_PORT
        0x30860314, //SEQ_DATA_PORT
        0x30860020, //SEQ_DATA_PORT
        0x30860314, //SEQ_DATA_PORT
        0x30860050, //SEQ_DATA_PORT
        0x30860414, //SEQ_DATA_PORT
        0x30860020, //SEQ_DATA_PORT
        0x30860414, //SEQ_DATA_PORT
        0x30860050, //SEQ_DATA_PORT
        0x30860514, //SEQ_DATA_PORT
        0x30860020, //SEQ_DATA_PORT
        0x30862405, //SEQ_DATA_PORT
        0x30861400, //SEQ_DATA_PORT
        0x30865001, //SEQ_DATA_PORT
        0x30862550, //SEQ_DATA_PORT
        0x3086502D, //SEQ_DATA_PORT
        0x30862608, //SEQ_DATA_PORT
        0x3086280D, //SEQ_DATA_PORT
        0x30861709, //SEQ_DATA_PORT
        0x30862600, //SEQ_DATA_PORT
        0x30862805, //SEQ_DATA_PORT
        0x308626A7, //SEQ_DATA_PORT
        0x30862807, //SEQ_DATA_PORT
        0x30862580, //SEQ_DATA_PORT
        0x30868029, //SEQ_DATA_PORT
        0x30862500, //SEQ_DATA_PORT
        0x30864027, //SEQ_DATA_PORT
        0x30860216, //SEQ_DATA_PORT
        0x30861627, //SEQ_DATA_PORT
        0x30860620, //SEQ_DATA_PORT
        0x30861736, //SEQ_DATA_PORT
        0x308626A6, //SEQ_DATA_PORT
        0x30861703, //SEQ_DATA_PORT
        0x308626A4, //SEQ_DATA_PORT
        0x3086171F, //SEQ_DATA_PORT
        0x30862805, //SEQ_DATA_PORT
        0x30862620, //SEQ_DATA_PORT
        0x30862804, //SEQ_DATA_PORT
        0x30862520, //SEQ_DATA_PORT
        0x30862027, //SEQ_DATA_PORT
        0x30860017, //SEQ_DATA_PORT
        0x30861D25, //SEQ_DATA_PORT
        0x30860020, //SEQ_DATA_PORT
        0x30861710, //SEQ_DATA_PORT
        0x30862805, //SEQ_DATA_PORT
        0x30861A17, //SEQ_DATA_PORT
        0x30860327, //SEQ_DATA_PORT
        0x30860617, //SEQ_DATA_PORT
        0x30860317, //SEQ_DATA_PORT
        0x30864126, //SEQ_DATA_PORT
        0x30866017, //SEQ_DATA_PORT
        0x3086AE25, //SEQ_DATA_PORT
        0x30860090, //SEQ_DATA_PORT
        0x30862700, //SEQ_DATA_PORT
        0x30862618, //SEQ_DATA_PORT
        0x30862800, //SEQ_DATA_PORT
        0x30862E2A, //SEQ_DATA_PORT
        0x30862808, //SEQ_DATA_PORT
        0x30861D05, //SEQ_DATA_PORT
        0x30861470, //SEQ_DATA_PORT
        0x30867009, //SEQ_DATA_PORT
        0x30861720, //SEQ_DATA_PORT
        0x30861400, //SEQ_DATA_PORT
        0x30862024, //SEQ_DATA_PORT
        0x30861400, //SEQ_DATA_PORT
        0x30865002, //SEQ_DATA_PORT
        0x30862550, //SEQ_DATA_PORT
        0x3086502D, //SEQ_DATA_PORT
        0x30862608, //SEQ_DATA_PORT
        0x3086280D, //SEQ_DATA_PORT
        0x30861709, //SEQ_DATA_PORT
        0x30862600, //SEQ_DATA_PORT
        0x30862805, //SEQ_DATA_PORT
        0x308626A7, //SEQ_DATA_PORT
        0x30862807, //SEQ_DATA_PORT
        0x30862580, //SEQ_DATA_PORT
        0x30868029, //SEQ_DATA_PORT
        0x30862500, //SEQ_DATA_PORT
        0x30864027, //SEQ_DATA_PORT
        0x30860216, //SEQ_DATA_PORT
        0x30861627, //SEQ_DATA_PORT
        0x30860617, //SEQ_DATA_PORT
        0x30863626, //SEQ_DATA_PORT
        0x3086A617, //SEQ_DATA_PORT
        0x30860326, //SEQ_DATA_PORT
        0x3086A417, //SEQ_DATA_PORT
        0x30861F28, //SEQ_DATA_PORT
        0x30860526, //SEQ_DATA_PORT
        0x30862028, //SEQ_DATA_PORT
        0x30860425, //SEQ_DATA_PORT
        0x30862020, //SEQ_DATA_PORT
        0x30862700, //SEQ_DATA_PORT
        0x3086171D, //SEQ_DATA_PORT
        0x30862500, //SEQ_DATA_PORT
        0x30862021, //SEQ_DATA_PORT
        0x30861710, //SEQ_DATA_PORT
        0x30862805, //SEQ_DATA_PORT
        0x30861B17, //SEQ_DATA_PORT
        0x30860327, //SEQ_DATA_PORT
        0x30860617, //SEQ_DATA_PORT
        0x30860317, //SEQ_DATA_PORT
        0x30864126, //SEQ_DATA_PORT
        0x30866017, //SEQ_DATA_PORT
        0x3086AE25, //SEQ_DATA_PORT
        0x30860090, //SEQ_DATA_PORT
        0x30862700, //SEQ_DATA_PORT
        0x30862618, //SEQ_DATA_PORT
        0x30862800, //SEQ_DATA_PORT
        0x30862E2A, //SEQ_DATA_PORT
        0x30862808, //SEQ_DATA_PORT
        0x30861E17, //SEQ_DATA_PORT
        0x30860A05, //SEQ_DATA_PORT
        0x30861470, //SEQ_DATA_PORT
        0x30867009, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861400, //SEQ_DATA_PORT
        0x30862024, //SEQ_DATA_PORT
        0x30861400, //SEQ_DATA_PORT
        0x3086502B, //SEQ_DATA_PORT
        0x3086302C, //SEQ_DATA_PORT
        0x30862C2C, //SEQ_DATA_PORT
        0x30862C00, //SEQ_DATA_PORT
        0x30860225,	//SEQ_DATA_PORT linear seq starts
        0x30865050,	//SEQ_DATA_PORT
        0x30862D26,	//SEQ_DATA_PORT
        0x30860828,	//SEQ_DATA_PORT
        0x30860D17,	//SEQ_DATA_PORT
        0x30860926,	//SEQ_DATA_PORT
        0x30860028,	//SEQ_DATA_PORT
        0x30860526,	//SEQ_DATA_PORT
        0x3086A728,	//SEQ_DATA_PORT
        0x30860725,	//SEQ_DATA_PORT
        0x30868080,	//SEQ_DATA_PORT
        0x30862917,	//SEQ_DATA_PORT
        0x30860525,	//SEQ_DATA_PORT
        0x30860040,	//SEQ_DATA_PORT
        0x30862702,	//SEQ_DATA_PORT
        0x30861616,	//SEQ_DATA_PORT
        0x30862706,	//SEQ_DATA_PORT
        0x30861736,	//SEQ_DATA_PORT
        0x308626A6,	//SEQ_DATA_PORT
        0x30861703,	//SEQ_DATA_PORT
        0x308626A4,	//SEQ_DATA_PORT
        0x3086171F,	//SEQ_DATA_PORT
        0x30862805,	//SEQ_DATA_PORT
        0x30862620,	//SEQ_DATA_PORT
        0x30862804,	//SEQ_DATA_PORT
        0x30862520,	//SEQ_DATA_PORT
        0x30862027,	//SEQ_DATA_PORT
        0x30860017,	//SEQ_DATA_PORT
        0x30861E25,	//SEQ_DATA_PORT
        0x30860020,	//SEQ_DATA_PORT
        0x30862117,	//SEQ_DATA_PORT
        0x30861028,	//SEQ_DATA_PORT
        0x3086051B,	//SEQ_DATA_PORT
        0x30861703,	//SEQ_DATA_PORT
        0x30862706,	//SEQ_DATA_PORT
        0x30861703,	//SEQ_DATA_PORT
        0x30861747,	//SEQ_DATA_PORT
        0x30862660,	//SEQ_DATA_PORT
        0x308617AE,	//SEQ_DATA_PORT
        0x30862500,	//SEQ_DATA_PORT
        0x30869027,	//SEQ_DATA_PORT
        0x30860026,	//SEQ_DATA_PORT
        0x30861828,	//SEQ_DATA_PORT
        0x3086002E,	//SEQ_DATA_PORT
        0x30862A28,	//SEQ_DATA_PORT
        0x3086081E,	//SEQ_DATA_PORT
        0x30860831,	//SEQ_DATA_PORT
        0x30861440,	//SEQ_DATA_PORT
        0x30864014,	//SEQ_DATA_PORT
        0x30862020,	//SEQ_DATA_PORT
        0x30861410,	//SEQ_DATA_PORT
        0x30861034,	//SEQ_DATA_PORT
        0x30861400,	//SEQ_DATA_PORT
        0x30861014,	//SEQ_DATA_PORT
        0x30860020,	//SEQ_DATA_PORT
        0x30861400,	//SEQ_DATA_PORT
        0x30864013,	//SEQ_DATA_PORT
        0x30861802,	//SEQ_DATA_PORT
        0x30861470,	//SEQ_DATA_PORT
        0x30867004,	//SEQ_DATA_PORT
        0x30861470,	//SEQ_DATA_PORT
        0x30867003,	//SEQ_DATA_PORT
        0x30861470,	//SEQ_DATA_PORT
        0x30867017,	//SEQ_DATA_PORT
        0x30862002,	//SEQ_DATA_PORT
        0x30861400,	//SEQ_DATA_PORT
        0x30862002,	//SEQ_DATA_PORT
        0x30861400,	//SEQ_DATA_PORT
        0x30865004,	//SEQ_DATA_PORT
        0x30861400,	//SEQ_DATA_PORT
        0x30862004,	//SEQ_DATA_PORT
        0x30861400,	//SEQ_DATA_PORT
        0x30865022,	//SEQ_DATA_PORT
        0x30860314,	//SEQ_DATA_PORT
        0x30860020,	//SEQ_DATA_PORT
        0x30860314,	//SEQ_DATA_PORT
        0x30860050,	//SEQ_DATA_PORT
        0x30862C2C,	//SEQ_DATA_PORT
        0x30862C2C,	//SEQ_DATA_PORT
        0x309E018A, //set start address for linear seq

//[A-1000ERS Rev3 Hidy and linear sequencer load December 16 2010]//
        0xFFFE00C8,
//Load = A-1000ERS Rev3 Optimized settings
//[A-1000ERS Rev3 Optimized settings]
        0x301E00A8, // set datapedestal to 168
        0x3EDA0F03, //Set vln_dac to 0x3 as recommended by Sergey
        0x3EDEC007,
        0x3ED801EF, // Vrst_low = +1
        0x3EE2A46B,
        //0x3EE0067D, // enable anti eclipse and adjust setting for high conversion gain
        0x3EE0047D, // enable anti eclipse and adjust setting for high conversion gain, decrease vertical line.
        0x3EDC0070, // adjust anti eclipse setting for low conversion gain
        0x30440404, //enable digital row noise correction and cancels TX during column correction
        0x3EE64303, // Helps with column noise at low light
        0x3EE4D208, // enable analog row noise correction
        0x3ED600BD,
        //0x3EE68303, //improves low light FPN

        // ADC settings to improve noise performance
        // c55300 comment: resolve defect pixel problem.
        0x30E46372,
        0x30E27253,
        0x30E05470,
        0x30E6C4CC,
        0x30E88050,
//[A-1000ERS Rev3 Optimized settings]//

//[HDR Mode Setup]//


//LOAD = Enable Parallel Mode //detect HSSAB board
//[Enable Parallel Mode]
        0x301A10D8, // Disable Streaming and setup parallel
        0x31D00001, // Set to 12 bits
//Load = PLL Enabled 27Mhz to 74.25Mhz
//[PLL Enabled 27Mhz to 74.25Mhz]
        0x302C0002, // VT_SYS_CLK_DIV
        0x302A0004, // VT_PIX_CLK_DIV 
        0x302E0002, // PRE_PLL_CLK_DIV
        0x3030002C, // PLL_MULTIPLIER
        0x30B00000, // Default is 0x1300????
        0xFFFE0064, 
//[PLL Enabled 27Mhz to 74.25Mhz]//
//[Enable Parallel Mode]//
//Load = Linear Mode Full Resolution
//[Linear Mode Full Resolution]
//LOAD= Linear Mode Setup
//[Linear Mode Setup]
        0x301A10D8, // stop streaming
        0x30820029, // Set Linear Mode register

//LOAD = Enable AE and Load Optimized Settings For HDR 16x Mode
//[Enable AE and Load Optimized Settings For Linear Mode]

//Load = Column Retriggering at start up
//[Column Retriggering at start up]
        0x30B01300, //disable AGS, set Column gain to 1x
        0x30D4E007, //enable double sampling for column correction
        0x30BA0008, // disable auto re-trigger for change in DCG and col gain and enable col corr always
        0x301A10DC, // enable streaming
        0xFFFE00C8, 
        0x301A10D8, // disable streaming
        0xFFFE00C8,
//[Column Retriggering at start up]//

        0x301202A0, // coarse integration time

//Load = 720p 30fps Setup
//[720p 30fps Setup]
        0x30320000, // digital binning
        0x30020002, // y_addr_start 
        0x30040000, // x_addr_start
        0x300602D1, // y_addr_end 
        0x300804FF, // x_addr_end
        0x300A02EE, // frame_length_lines 
        0x300C0F78, // line_length_pck
//[720p 30fps Setup]//

//Load= Enable Embedded Data and Stats
//[Enable Embedded Data and Stats]
        //0x30641982, //0x30641800
//[Enable Embedded Data and Stats]//

//[Disable Embedded Data and Stats]
        0x30641802, 

//remove vertical line, put here can not take effect. Why????
//[Enable AE and Load Optimized Settings For Linear Mode]//
        0x3100001B, //FIELD_WR=AECTRLREG, 0x001B
        0x3112029F, //FIELD_WR=AEDCGEXPOSUREHIGHREG, 0x029F
        0x3114008C, //FIELD_WR=AEDCGEXPOSURELOWREG, 0x008C
        0x311602C0, //FIELD_WR=AEDCGGAINFACTOR, 0x02C0
        0x3118005B, //FIELD_WR=AEDCGGAININVREG, 0x005B
        0x31020384, //FIELD_WR=AELUMATARGETREG, 0x0384
        0x31041000, //FIELD_WR-AEHISTTARGETREG, 0x1000
        0x31260080, //FIELD_WR=AEALPHAV1REG, 0x0080
        0x311C03DD, //FIELD_WR=AEMAXEXPOSUREREG, 0x03C0
        0x311E0002, //FIELD_WR=AEMINEXPOSUREREG, 0x0002
//[Enable AE and Load Optimized Settings For Linear Mode]//

//[Disable_AutoExposure]
        0x3100001A,      //FIELD_WR=AECTRLREG, 0x001A ????

        0x301A10DC, //enable streaming.


//[Linear Mode Full Resolution]//
        0xFFFFFFFF  // END
};


//9m034 720P 30fps wdr
static int sensor_rom_30_wdr[] = {
        // [Linear Mode 720p 30fps AE=on EMBEDDED=on]
//[Demo Initialization]
//Load = Reset
//[Reset]
        0xFFFE00C8, // wait 8 frames
        0x301A10D9,
		0xFFFE0005, // delay 5ms after sensor reset
        0x301A10D8,
//[Reset]//
        0xFFFE00C8,
//Load = A-1000ERS Rev3 Hidy and linear sequencer load December 16 2010
//[A-1000ERS Rev3 Hidy and linear sequencer load December 16 2010]
//HiDY sequencer CR 31644
        0x30888000, //SEQ_CTRL_PORT
        0x30860025, //SEQ_DATA_PORT
        0x30865050, //SEQ_DATA_PORT
        0x30862D26, //SEQ_DATA_PORT
        0x30860828, //SEQ_DATA_PORT
        0x30860D17, //SEQ_DATA_PORT
        0x30860926, //SEQ_DATA_PORT
        0x30860028, //SEQ_DATA_PORT
        0x30860526, //SEQ_DATA_PORT
        0x3086A728, //SEQ_DATA_PORT
        0x30860725, //SEQ_DATA_PORT
        0x30868080, //SEQ_DATA_PORT
        0x30862925, //SEQ_DATA_PORT
        0x30860040, //SEQ_DATA_PORT
        0x30862702, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30862706, //SEQ_DATA_PORT

        0x30861F17, //SEQ_DATA_PORT
        0x30863626, //SEQ_DATA_PORT
        0x3086A617, //SEQ_DATA_PORT
        0x30860326, //SEQ_DATA_PORT
        0x3086A417, //SEQ_DATA_PORT
        0x30861F28, //SEQ_DATA_PORT
        0x30860526, //SEQ_DATA_PORT
        0x30862028, //SEQ_DATA_PORT
        0x30860425, //SEQ_DATA_PORT
        0x30862020, //SEQ_DATA_PORT
        0x30862700, //SEQ_DATA_PORT
        0x3086171D, //SEQ_DATA_PORT
        0x30862500, //SEQ_DATA_PORT
        0x30862017, //SEQ_DATA_PORT
        0x30861028, //SEQ_DATA_PORT
        0x30860519, //SEQ_DATA_PORT
        0x30861703, //SEQ_DATA_PORT
        0x30862706, //SEQ_DATA_PORT
        0x30861703, //SEQ_DATA_PORT
        0x30861741, //SEQ_DATA_PORT
        0x30862660, //SEQ_DATA_PORT
        0x3086175A, //SEQ_DATA_PORT
        0x30862317, //SEQ_DATA_PORT
        0x30861122, //SEQ_DATA_PORT
        0x30861741, //SEQ_DATA_PORT
        0x30862500, //SEQ_DATA_PORT
        0x30869027, //SEQ_DATA_PORT
        0x30860026, //SEQ_DATA_PORT
        0x30861828, //SEQ_DATA_PORT
        0x3086002E, //SEQ_DATA_PORT
        0x30862A28, //SEQ_DATA_PORT
        0x3086081C, //SEQ_DATA_PORT
        0x30861470, //SEQ_DATA_PORT
        0x30867003, //SEQ_DATA_PORT
        0x30861470, //SEQ_DATA_PORT
        0x30867004, //SEQ_DATA_PORT
        0x30861470, //SEQ_DATA_PORT
        0x30867005, //SEQ_DATA_PORT
        0x30861470, //SEQ_DATA_PORT
        0x30867009, //SEQ_DATA_PORT
        0x3086170C, //SEQ_DATA_PORT
        0x30860014, //SEQ_DATA_PORT
        0x30860020, //SEQ_DATA_PORT
        0x30860014, //SEQ_DATA_PORT
        0x30860050, //SEQ_DATA_PORT
        0x30860314, //SEQ_DATA_PORT
        0x30860020, //SEQ_DATA_PORT
        0x30860314, //SEQ_DATA_PORT
        0x30860050, //SEQ_DATA_PORT
        0x30860414, //SEQ_DATA_PORT
        0x30860020, //SEQ_DATA_PORT
        0x30860414, //SEQ_DATA_PORT
        0x30860050, //SEQ_DATA_PORT
        0x30860514, //SEQ_DATA_PORT
        0x30860020, //SEQ_DATA_PORT
        0x30862405, //SEQ_DATA_PORT
        0x30861400, //SEQ_DATA_PORT
        0x30865001, //SEQ_DATA_PORT
        0x30862550, //SEQ_DATA_PORT
        0x3086502D, //SEQ_DATA_PORT
        0x30862608, //SEQ_DATA_PORT
        0x3086280D, //SEQ_DATA_PORT
        0x30861709, //SEQ_DATA_PORT
        0x30862600, //SEQ_DATA_PORT
        0x30862805, //SEQ_DATA_PORT
        0x308626A7, //SEQ_DATA_PORT
        0x30862807, //SEQ_DATA_PORT
        0x30862580, //SEQ_DATA_PORT
        0x30868029, //SEQ_DATA_PORT
        0x30862500, //SEQ_DATA_PORT
        0x30864027, //SEQ_DATA_PORT
        0x30860216, //SEQ_DATA_PORT
        0x30861627, //SEQ_DATA_PORT
        0x30860620, //SEQ_DATA_PORT
        0x30861736, //SEQ_DATA_PORT
        0x308626A6, //SEQ_DATA_PORT
        0x30861703, //SEQ_DATA_PORT
        0x308626A4, //SEQ_DATA_PORT
        0x3086171F, //SEQ_DATA_PORT
        0x30862805, //SEQ_DATA_PORT
        0x30862620, //SEQ_DATA_PORT
        0x30862804, //SEQ_DATA_PORT
        0x30862520, //SEQ_DATA_PORT
        0x30862027, //SEQ_DATA_PORT
        0x30860017, //SEQ_DATA_PORT
        0x30861D25, //SEQ_DATA_PORT
        0x30860020, //SEQ_DATA_PORT
        0x30861710, //SEQ_DATA_PORT
        0x30862805, //SEQ_DATA_PORT
        0x30861A17, //SEQ_DATA_PORT
        0x30860327, //SEQ_DATA_PORT
        0x30860617, //SEQ_DATA_PORT
        0x30860317, //SEQ_DATA_PORT
        0x30864126, //SEQ_DATA_PORT
        0x30866017, //SEQ_DATA_PORT
        0x3086AE25, //SEQ_DATA_PORT
        0x30860090, //SEQ_DATA_PORT
        0x30862700, //SEQ_DATA_PORT
        0x30862618, //SEQ_DATA_PORT
        0x30862800, //SEQ_DATA_PORT
        0x30862E2A, //SEQ_DATA_PORT
        0x30862808, //SEQ_DATA_PORT
        0x30861D05, //SEQ_DATA_PORT
        0x30861470, //SEQ_DATA_PORT
        0x30867009, //SEQ_DATA_PORT
        0x30861720, //SEQ_DATA_PORT
        0x30861400, //SEQ_DATA_PORT
        0x30862024, //SEQ_DATA_PORT
        0x30861400, //SEQ_DATA_PORT
        0x30865002, //SEQ_DATA_PORT
        0x30862550, //SEQ_DATA_PORT
        0x3086502D, //SEQ_DATA_PORT
        0x30862608, //SEQ_DATA_PORT
        0x3086280D, //SEQ_DATA_PORT
        0x30861709, //SEQ_DATA_PORT
        0x30862600, //SEQ_DATA_PORT
        0x30862805, //SEQ_DATA_PORT
        0x308626A7, //SEQ_DATA_PORT
        0x30862807, //SEQ_DATA_PORT
        0x30862580, //SEQ_DATA_PORT
        0x30868029, //SEQ_DATA_PORT
        0x30862500, //SEQ_DATA_PORT
        0x30864027, //SEQ_DATA_PORT
        0x30860216, //SEQ_DATA_PORT
        0x30861627, //SEQ_DATA_PORT
        0x30860617, //SEQ_DATA_PORT
        0x30863626, //SEQ_DATA_PORT
        0x3086A617, //SEQ_DATA_PORT
        0x30860326, //SEQ_DATA_PORT
        0x3086A417, //SEQ_DATA_PORT
        0x30861F28, //SEQ_DATA_PORT
        0x30860526, //SEQ_DATA_PORT
        0x30862028, //SEQ_DATA_PORT
        0x30860425, //SEQ_DATA_PORT
        0x30862020, //SEQ_DATA_PORT
        0x30862700, //SEQ_DATA_PORT
        0x3086171D, //SEQ_DATA_PORT
        0x30862500, //SEQ_DATA_PORT
        0x30862021, //SEQ_DATA_PORT
        0x30861710, //SEQ_DATA_PORT
        0x30862805, //SEQ_DATA_PORT
        0x30861B17, //SEQ_DATA_PORT
        0x30860327, //SEQ_DATA_PORT
        0x30860617, //SEQ_DATA_PORT
        0x30860317, //SEQ_DATA_PORT
        0x30864126, //SEQ_DATA_PORT
        0x30866017, //SEQ_DATA_PORT
        0x3086AE25, //SEQ_DATA_PORT
        0x30860090, //SEQ_DATA_PORT
        0x30862700, //SEQ_DATA_PORT
        0x30862618, //SEQ_DATA_PORT
        0x30862800, //SEQ_DATA_PORT
        0x30862E2A, //SEQ_DATA_PORT
        0x30862808, //SEQ_DATA_PORT
        0x30861E17, //SEQ_DATA_PORT
        0x30860A05, //SEQ_DATA_PORT
        0x30861470, //SEQ_DATA_PORT
        0x30867009, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861616, //SEQ_DATA_PORT
        0x30861400, //SEQ_DATA_PORT
        0x30862024, //SEQ_DATA_PORT
        0x30861400, //SEQ_DATA_PORT
        0x3086502B, //SEQ_DATA_PORT
        0x3086302C, //SEQ_DATA_PORT
        0x30862C2C, //SEQ_DATA_PORT
        0x30862C00, //SEQ_DATA_PORT
        0x30860225,	//SEQ_DATA_PORT linear seq starts
        0x30865050,	//SEQ_DATA_PORT
        0x30862D26,	//SEQ_DATA_PORT
        0x30860828,	//SEQ_DATA_PORT
        0x30860D17,	//SEQ_DATA_PORT
        0x30860926,	//SEQ_DATA_PORT
        0x30860028,	//SEQ_DATA_PORT
        0x30860526,	//SEQ_DATA_PORT
        0x3086A728,	//SEQ_DATA_PORT
        0x30860725,	//SEQ_DATA_PORT
        0x30868080,	//SEQ_DATA_PORT
        0x30862917,	//SEQ_DATA_PORT
        0x30860525,	//SEQ_DATA_PORT
        0x30860040,	//SEQ_DATA_PORT
        0x30862702,	//SEQ_DATA_PORT
        0x30861616,	//SEQ_DATA_PORT
        0x30862706,	//SEQ_DATA_PORT
        0x30861736,	//SEQ_DATA_PORT
        0x308626A6,	//SEQ_DATA_PORT
        0x30861703,	//SEQ_DATA_PORT
        0x308626A4,	//SEQ_DATA_PORT
        0x3086171F,	//SEQ_DATA_PORT
        0x30862805,	//SEQ_DATA_PORT
        0x30862620,	//SEQ_DATA_PORT
        0x30862804,	//SEQ_DATA_PORT
        0x30862520,	//SEQ_DATA_PORT
        0x30862027,	//SEQ_DATA_PORT
        0x30860017,	//SEQ_DATA_PORT
        0x30861E25,	//SEQ_DATA_PORT
        0x30860020,	//SEQ_DATA_PORT
        0x30862117,	//SEQ_DATA_PORT
        0x30861028,	//SEQ_DATA_PORT
        0x3086051B,	//SEQ_DATA_PORT
        0x30861703,	//SEQ_DATA_PORT
        0x30862706,	//SEQ_DATA_PORT
        0x30861703,	//SEQ_DATA_PORT
        0x30861747,	//SEQ_DATA_PORT
        0x30862660,	//SEQ_DATA_PORT
        0x308617AE,	//SEQ_DATA_PORT
        0x30862500,	//SEQ_DATA_PORT
        0x30869027,	//SEQ_DATA_PORT
        0x30860026,	//SEQ_DATA_PORT
        0x30861828,	//SEQ_DATA_PORT
        0x3086002E,	//SEQ_DATA_PORT
        0x30862A28,	//SEQ_DATA_PORT
        0x3086081E,	//SEQ_DATA_PORT
        0x30860831,	//SEQ_DATA_PORT
        0x30861440,	//SEQ_DATA_PORT
        0x30864014,	//SEQ_DATA_PORT
        0x30862020,	//SEQ_DATA_PORT
        0x30861410,	//SEQ_DATA_PORT
        0x30861034,	//SEQ_DATA_PORT
        0x30861400,	//SEQ_DATA_PORT
        0x30861014,	//SEQ_DATA_PORT
        0x30860020,	//SEQ_DATA_PORT
        0x30861400,	//SEQ_DATA_PORT
        0x30864013,	//SEQ_DATA_PORT
        0x30861802,	//SEQ_DATA_PORT
        0x30861470,	//SEQ_DATA_PORT
        0x30867004,	//SEQ_DATA_PORT
        0x30861470,	//SEQ_DATA_PORT
        0x30867003,	//SEQ_DATA_PORT
        0x30861470,	//SEQ_DATA_PORT
        0x30867017,	//SEQ_DATA_PORT
        0x30862002,	//SEQ_DATA_PORT
        0x30861400,	//SEQ_DATA_PORT
        0x30862002,	//SEQ_DATA_PORT
        0x30861400,	//SEQ_DATA_PORT
        0x30865004,	//SEQ_DATA_PORT
        0x30861400,	//SEQ_DATA_PORT
        0x30862004,	//SEQ_DATA_PORT
        0x30861400,	//SEQ_DATA_PORT
        0x30865022,	//SEQ_DATA_PORT
        0x30860314,	//SEQ_DATA_PORT
        0x30860020,	//SEQ_DATA_PORT
        0x30860314,	//SEQ_DATA_PORT
        0x30860050,	//SEQ_DATA_PORT
        0x30862C2C,	//SEQ_DATA_PORT
        0x30862C2C,	//SEQ_DATA_PORT
        0x309E018A, //set start address for linear seq
//[A-1000ERS Rev3 Hidy and linear sequencer load December 16 2010]//

        0xFFFE00C8,
//Load = A-1000ERS Rev3 Optimized settings
//[A-1000ERS Rev3 Optimized settings]
        0x301E00C0, // set datapedestal to 192 to avoid clipping near saturation
        0x3EDA0F03, //Set vln_dac to 0x3 as recommended by Sergey
        0x3EDEC007,
        0x3ED801EF, // Vrst_low = +1
        0x3EE2A46B,
        //0x3EE0067D, // enable anti eclipse and adjust setting for high conversion gain
        0x3EE0047D, // enable anti eclipse and adjust setting for high conversion gain, decrease vertical line.
        0x3EDC0070, // adjust anti eclipse setting for low conversion gain
        0x30440404, //enable digital row noise correction and cancels TX during column correction
        0x3EE64303, // Helps with column noise at low light
        0x3EE4D208, // enable analog row noise correction
        0x3ED600BD,
        //0x3EE68303, //improves low light FPN

        // ADC settings to improve noise performance
        // c55300 comment: resolve defect pixel problem.
        0x30E46372,
        0x30E27253,
        0x30E05470,
        0x30E6C4CC,
        0x30E88050,
//[A-1000ERS Rev3 Optimized settings]//

//LOAD = Enable Parallel Mode //detect HSSAB board
//[Enable Parallel Mode]
        0x301A10D8, // Disable Streaming and setup parallel
        0x31D00001, // Set to 12 bits
    //Load = PLL Enabled 27Mhz to 74.25Mhz
    //[PLL Enabled 27Mhz to 74.25Mhz]
        0x302C0002, // VT_SYS_CLK_DIV
        0x302A0004, // VT_PIX_CLK_DIV 
        0x302E0002, // PRE_PLL_CLK_DIV
        0x3030002C, // PLL_MULTIPLIER
        0x30B00000, //????
        0xFFFE0064, 
    //[PLL Enabled 27Mhz to 74.25Mhz]//
//[Enable Parallel Mode]//

//Load = HiDy 16x Mode Full Resolution
//[HiDy 16x Mode Full Resolution]
//Load = HDR Mode Setup
//[HDR Mode Setup]
        0x301A10D8, // stop streaming
        0x30820028, // Set HiDy 16x ratio
//[HDR Mode Setup]//

//Load = Column Retriggering at start up
//[Column Retriggering at start up]
        0x30B01300, //disable AGS, set Column gain to 1x
        0x30D4E007, //enable double sampling for column correction
        0x30BA0008, // disable auto re-trigger for change in DCG and col gain and enable col corr always
        0x301A10DC, // enable streaming
        0xFFFE00C8, 
        0x301A10D8, // disable streaming
        0xFFFE00C8,
//[Column Retriggering at start up]//

        0x301202A0, // coarse integration time

//Load = 720p 25fps Setup
//[720p 25fps Setup]
        0x30320000, // digital binning
        0x30020002, // y_addr_start 
        0x30040000, // x_addr_start
        0x300602D1, // y_addr_end
        //0x300603C1, // y_addr_end
        0x300804FF, // x_addr_end
        0x300A02EE, // frame_length_lines 
        0x300C0F78, // line_length_pck
//[720p 30fps Setup]//

//LOAD = Enable AE and Load Optimized Settings For HDR 16x Mode
//Load= Enable Embedded Data and Stats
//[Enable Embedded Data and Stats]
		//0x30641982, //0x30641800,
//[Enable Embedded Data and Stats]//

//[Disable Embedded Data and Stats]
        0x30641802, 

//[Enable AE and Load Optimized Settings For HDR 16x Mode]//
        0x3100001B,      //FIELD_WR=AECTRLREG, 0x001B                 
        0x3112029F,      //FIELD_WR=AEDCGEXPOSUREHIGHREG, 0x029F      
        0x3114008C,      //FIELD_WR=AEDCGEXPOSURELOWREG, 0x0190       
        0x311602C0,      //FIELD_WR=AEDCGGAINFACTOR, 0x02C0           
        0x3118005B,      //FIELD_WR=AEDCGGAININVREG, 0x005B           
        0x31020708,      //FIELD_WR=AELUMATARGETREG, 0x0708           
        0x31041000,     //FIELD_WR-AEHISTTARGETREG, 0x1000   
        0x31260064,      //FIELD_WR=AEALPHAV1REG, 0x0064              
        0x311C02A0,      //FIELD_WR=AEMAXEXPOSUREREG, 0x02A0          
        0x311E0080,      //FIELD_WR=AEMINEXPOSUREREG, 0x0080          

        //[2D motion compensation ON]    
        0x318A0FA0, 	// HDR_MC_CTRL1
        0x31920800, 	// HDR_MC_CTRL5
        0x318CC340, 	// HDR_MC_CTRL2
        0x318E0320, 	// HDR_MC_CTRL3
        // [Disable DLO]1: REG=0x3190, 0x0BA0
        0x31900BA0, 	// HDR_MC_CTRL4

//[Disable_AutoExposure]
        0x3100001A,      //FIELD_WR=AECTRLREG, 0x001A

        0x301A10DC, //enable streaming.
        

//[HiDy 16x Mode Full Resolution]//
        0xFFFFFFFF  // END
};



