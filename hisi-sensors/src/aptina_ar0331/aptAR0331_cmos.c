#if !defined(__AR0331_CMOS_H_)
#define __AR0331_CMOS_H_

#include <stdio.h>
#include <string.h>
#include "hi_comm_sns.h"
#include "hi_sns_ctrl.h"
#include "mpi_isp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/****************************************************************************
 * local variables															*
 ****************************************************************************/

static cmos_inttime_t cmos_inttime;
static cmos_gains_t cmos_gains;

static cmos_isp_default_t st_coms_isp_default_lin = 
{
#if 1	//V2
	// color matrix[9]

	{	0x0189, 0x804A, 0x8041,
		0x802E, 0x0155, 0x8029,
		0x000B, 0x8066, 0x0159
	},

	// black level(linear)
	{0xa8,0xa8,0xa8,0xa8},
	
	//calibration reference color temperature 
	5000,

	//WB gain at 5000K, must keep consistent with calibration color temperature 
	{0x100 + 0x42, 0x100 + 0x00, 0x100 + 0x00, 0x100 + 0xA0},

	// WB curve parameters, must keep consistent with reference color temperature.
	{-478, 1062, 328, 218467, 128, -158761},
#else	//V1
	// color matrix[9]

	{	0x0168, 0x802D, 0x803C,
		0x801D, 0x0142, 0x8025,
		0x0014, 0x804C, 0x0137
	},

	// black level(linear)
	{0xa8,0xa8,0xa8,0xa8},
	
	//calibration reference color temperature 
	5000,

	//WB gain at 5000K, must keep consistent with calibration color temperature 
	{0x100 + 0x3E, 0x100 + 0x00, 0x100 + 0x00, 0x100 + 0xB2},

	// WB curve parameters, must keep consistent with reference color temperature.
	{-9, 203, -62, 217629, 128, -164063},
#endif
	// hist_thresh
	{0x07,0x0C,0x80,0x84},
	
	0x00,	// iridix_balck
	0x1,	// rggb

	// gain
	0x8,	0x10, // this is gain target, it will be constricted by sensor-gain.

	//wdr_variance_space, wdr_variance_intensity, slope_max_write,  white_level_write
	0x04,	0x01,	0x30, 	0x4FF,
	
	0x1, 	// balance_fe
	0x70,	// ae compensation
	0x15, 	// sinter threshold

	0x1,    //0: use default profile table; 1: use calibrated profile lut, the setting for nr0 and nr1 must be correct.
	0,      //nr0
	1404    //nr1  
};

static cmos_isp_default_t st_coms_isp_default_wdr = 
{
#if 1	//V2
	// color matrix[9]

	{	0x019C, 0x805A, 0x8045,
		0x8037, 0x015F, 0x802B,
		0x0006, 0x8077, 0x016F
	},

	// black level(wdr)
	{0x00, 0x00, 0x00, 0x00},
	
	//calibration reference color temperature 
	5000,

	//WB gain at 5000K, must keep consistent with calibration color temperature 
	{0x100 + 0x42, 0x100 + 0x00, 0x100 + 0x00, 0x100 + 0xA0},

	// WB curve parameters, must keep consistent with reference color temperature.
	{-478, 1062, 328, 218467, 128, -158761},	// color matrix[9]

#else	//V1
	// color matrix[9]

	{	0x01BB, 0x806F, 0x804E,
		0x8043, 0x0173, 0x8030,
		0x0003, 0x8098, 0x0198
	},

	// black level(wdr)
	{0x00, 0x00, 0x00, 0x00},
	
	//calibration reference color temperature 
	5000,

	//WB gain at 5000K, must keep consistent with calibration color temperature 
	{0x100 + 0x3E, 0x100 + 0x00, 0x100 + 0x00, 0x100 + 0xB2},

	// WB curve parameters, must keep consistent with reference color temperature.
	{-9, 203, -62, 217629, 128, -164063},
#endif

	// hist_thresh
	{0x07,0x0C,0x80,0x84},
	
	0x00,	// iridix_balck
	0x1,	// rggb

	// gain
	0x8,	0x10,

	//wdr_variance_space, wdr_variance_intensity, slope_max_write,  white_level_write
	0x04,	0x01,	0x40, 	0xCFF,
	
	0x0, 	// balance_fe
	0x70,	// ae compensation
	0x15, 	// sinter threshold

	0x1,    //0: use strength; 1: use profile lut.
	0,      //nr0
	1404  //nr1  
};

/*
 * This function initialises an instance of cmos_inttime_t.
 */
static /*__inline*/ cmos_inttime_const_ptr_t cmos_inttime_initialize()
{
    //TODO: min/max integration time control.
   	//cmos_inttime.min_lines_std = 128;
#if defined(APTAR0331_WDR_CMOS)
	//HDR mode,maximum coarse_integration_time = minimum(70 x T1 / T2, frame_length_lines - 71)
	cmos_inttime.full_lines_std = 1056;
#else
	cmos_inttime.full_lines_std = 1125;
#endif	
	cmos_inttime.full_lines_std_30fps = 1125;
	cmos_inttime.full_lines = 1125;
	cmos_inttime.full_lines_del = 1125; //TODO: remove
	cmos_inttime.full_lines_limit = 65535;
	//HDR mode,maximum coarse_integration_time = minimum(70 x T1 / T2, frame_length_lines - 71)
	cmos_inttime.max_lines = 1054;
	cmos_inttime.min_lines = 8;
	cmos_inttime.vblanking_lines = 1125;

	cmos_inttime.exposure_ashort = 0;
	cmos_inttime.exposure_shift = 0;

	cmos_inttime.lines_per_500ms = 1125*30/2; 
	cmos_inttime.flicker_freq = 0;//60*256;//50*256;

	cmos_inttime.max_lines_target = cmos_inttime.max_lines;
	cmos_inttime.min_lines_target = cmos_inttime.min_lines;
	//cmos_inttime.max_flicker_lines = cmos_inttime.max_lines_target;
	//cmos_inttime.min_flicker_lines = cmos_inttime.min_lines_target;
	//cmos_inttime.input_changed = 0;

	return &cmos_inttime;
}

/*
 * This function applies the new integration time to the ISP registers.
 */
static __inline void cmos_inttime_update(cmos_inttime_ptr_t p_inttime) 
{
	HI_U16 _time = p_inttime->exposure_ashort >> p_inttime->exposure_shift;
	sensor_write_register(0x3012, _time);
}

/*
 * This function applies the new vert blanking porch to the ISP registers.
 */
static __inline void cmos_vblanking_update(cmos_inttime_const_ptr_t p_inttime)
{
}

static __inline HI_U16 vblanking_calculate(
		cmos_inttime_ptr_t p_inttime)
{
	p_inttime->exposure_along  = p_inttime->exposure_ashort;

	if(p_inttime->exposure_along < p_inttime->full_lines_std - 2)
	{
		p_inttime->full_lines_del = p_inttime->full_lines_std;
	}
	if(p_inttime->exposure_along >= p_inttime->full_lines_std - 2)
	{
		p_inttime->full_lines_del = p_inttime->exposure_along + 2;
	}
#if defined(TRACE_ALL)
	alt_printf("full_lines_del = %x\n", p_inttime->full_lines_del);
#endif
	p_inttime->vblanking_lines = p_inttime->full_lines_del - p_inttime->full_lines_std_30fps;
#if defined(TRACE_ALL)
	alt_printf("vblanking_lines = %x\n", p_inttime->vblanking_lines);
#endif
	return p_inttime->exposure_ashort;
}

/* Set fps base */
static /*__inline*/ void cmos_fps_set(
		cmos_inttime_ptr_t p_inttime,
		const HI_U8 fps
		)
{
	switch(fps)
	{
		default:// default 30fps
		case 30:
			sensor_write_register(0x300C, 0x044C);
			p_inttime->lines_per_500ms = 1125 * 30 / 2;
		break;
		case 25:
			sensor_write_register(0x300C, 0x0528);
			p_inttime->lines_per_500ms = 1125 * 25 / 2;
		break;
	}
}

/*
 * This function initialises an instance of cmos_gains_t.
 */
static /*__inline*/ cmos_gains_ptr_t cmos_gains_initialize()
{
    // ???? why not use max analog in WDR mode.
#if defined(APTAR0331_WDR_CMOS)
	cmos_gains.max_again = 8; // anti-flicker is off.
    //cmos_gains.max_again = 1; // anti-flicker is on.
#else
	cmos_gains.max_again = 8;
#endif

	cmos_gains.max_dgain = 255;
	cmos_gains.max_again_target = cmos_gains.max_again;
	cmos_gains.max_dgain_target = cmos_gains.max_dgain;

	cmos_gains.again_shift = 0;
	cmos_gains.dgain_shift = 4;//actual dgain shift is 7, so dgain should <<3 before writed.
	cmos_gains.dgain_fine_shift = 0;

	cmos_gains.again = (1 << cmos_gains.again_shift);
	cmos_gains.dgain = (1 << cmos_gains.dgain_shift);
	cmos_gains.dgain_fine = (1 << cmos_gains.dgain_shift);
	cmos_gains.again_db = 0;
	cmos_gains.dgain_db = 0;

//	cmos_gains.input_changed = 0;

	return &cmos_gains;
}

/*
 * This function applies the new gains to the ISP registers.
 */
static __inline void cmos_gains_update(cmos_gains_const_ptr_t p_gains)
{
	int ag = p_gains->again; 
	int dg = p_gains->dgain;
	
	switch(ag)
	{
		case(0):
			sensor_write_register(0x3060, 0x1300);  //AG
			//sensor_write_register(0x3198, 0x201E);  //motion_detect_q1
			break;
		case(1):
			sensor_write_register(0x3060, 0x1300);
			//sensor_write_register(0x3198, 0x201E);
			break;
		case(2):
			sensor_write_register(0x3060, 0x1310);
			//sensor_write_register(0x3198, 0x202A);
			break;
		case(4):
			sensor_write_register(0x3060, 0x1320);
			//sensor_write_register(0x3198, 0x203C);
			break;
		case(8):
			sensor_write_register(0x3060, 0x1330);
			//sensor_write_register(0x3198, 0x2055);
			break;
	}
	
	sensor_write_register(0x305E, dg << 3);
}

/* Emulate digital fine gain */
static __inline void em_dgain_fine_update(cmos_gains_ptr_t p_gains)
{
}

static HI_U32 cmos_gains_lin_to_db_convert(HI_U32 data, HI_U32 shift_in)
{
    #define PRECISION 8
	HI_U32 _res = 0;
	if(0 == data)
		return _res;

    data = data << PRECISION; // to ensure precision.
	for(;;)
	{
        /* Note to avoid endless loop here. */
		data = (data * 913) >> 10;
        // data = (data*913 + (1<<9)) >> 10; // endless loop when shift_in is 0. */
		if(data <= ((1<<shift_in) << PRECISION))
		{
			break;
		}
		++_res;
	}
	return _res;
}

static __inline HI_U32 analog_gain_from_exposure_calculate(
		cmos_gains_ptr_t p_gains,
		HI_U32 exposure,
		HI_U32 exposure_max,
		HI_U32 exposure_shift)
{
	HI_U32 _again = 1<<p_gains->again_shift;
	//HI_U32 _ares = 1<<p_gains->again_shift;
	//HI_U32 _lres = 0;
	int shft = 0;

	while (exposure > (1<<24))
	{
		exposure >>= 1;
		exposure_max >>= 1;
		++shft;
	}

	if(exposure > exposure_max)
	{
                //when setting manual exposure line, exposure_max>>shift should not be 0.
                exposure_max = DIV_0_TO_1(exposure_max);
		_again = (exposure  * _again)  / exposure_max;
//		exposure = exposure_max;

		if (_again >= 1<< 3) { _again = 1<<3; }
		else if (_again >= 1<< 2) { _again = 1<<2; }
		else if (_again >= 1<< 1) { _again = 1<<1; }
		else if (_again >= 1)     { _again = 1;    }

		_again = _again < (1<<p_gains->again_shift) ? (1<<p_gains->again_shift) : _again;
		_again = _again > (HI_U32)(1<<p_gains->again_shift) * p_gains->max_again_target ?
			(HI_U32)(1<<p_gains->again_shift) * p_gains->max_again_target : _again;
		
		exposure = (exposure / _again);
	}
	else
	{
		//_again = (_again * exposure) / (exposure / exposure_shift) / exposure_shift;
	}

	p_gains->again = _again;
    p_gains->again_db = cmos_gains_lin_to_db_convert(p_gains->again, p_gains->again_shift);
	return (exposure << shft);
}

static __inline HI_U32 digital_gain_from_exposure_calculate(
		cmos_gains_ptr_t p_gains,
		HI_U32 exposure,
		HI_U32 exposure_max,
		HI_U32 exposure_shift
		)
{
	HI_U32 _dgain = 1<<p_gains->dgain_shift;
	int shft = 0;

	while (exposure > (1<<24))
	{
		exposure >>= 1;
		exposure_max >>= 1;
		++shft;
	}

	if(exposure > exposure_max)
	{
	    //when setting manual exposure line, exposure_max>>shift should not be 0.
            exposure_max = DIV_0_TO_1(exposure_max);
            _dgain = (exposure  * _dgain) / exposure_max;
            exposure = exposure_max;
	}
	else
	{
        //TODO: after anti-flick, dgain may need to decrease. 
		//_dgain = (_dgain * exposure) / (exposure / exposure_shift) / exposure_shift;
	}
	_dgain = _dgain < (1<<p_gains->dgain_shift) ? (1<<p_gains->dgain_shift) : _dgain;
	_dgain = _dgain >  p_gains->max_dgain_target ? p_gains->max_dgain_target : _dgain;

	p_gains->dgain = _dgain;
    p_gains->dgain_db = cmos_gains_lin_to_db_convert(p_gains->dgain, p_gains->dgain_shift);

	return exposure << shft;
}

static __inline void sensor_update(
	cmos_gains_const_ptr_t p_gains,
	cmos_inttime_ptr_t p_inttime,
	HI_U8 frame
    )
{
	if(frame == 0)
	{
		cmos_inttime_update(p_inttime);
	}
	if(frame == 1)
	{
		cmos_gains_update(p_gains);
	}    
}

static __inline HI_U32 cmos_get_ISO(cmos_gains_ptr_t p_gains)
{
	HI_U32 _again = p_gains->again == 0 ? 1 : p_gains->again;
	HI_U32 _dgain = p_gains->dgain == 0 ? 1 : p_gains->dgain;

	p_gains->iso =  ((_again * _dgain * 100) >> (p_gains->again_shift + p_gains->dgain_shift));

	return p_gains->iso;
}

static HI_U8 cmos_get_analog_gain(cmos_gains_ptr_t p_gains)
{
    //return cmos_gains_lin_to_db_convert(p_gains->again, p_gains->again_shift);
    return p_gains->again_db;
}

static HI_U8 cmos_get_digital_gain(cmos_gains_ptr_t p_gains)
{
    //return cmos_gains_lin_to_db_convert(p_gains->dgain, p_gains->dgain_shift);
    return p_gains->dgain_db;
}

static HI_U8 cmos_get_digital_fine_gain(cmos_gains_ptr_t p_gains)
{
    return cmos_gains_lin_to_db_convert(p_gains->dgain_fine, p_gains->dgain_shift);
}

static HI_U32 cmos_get_isp_default(cmos_isp_default_ptr_t p_coms_isp_default)
{
	if (NULL == p_coms_isp_default)
	{
	    printf("null pointer when get isp default value!\n");
	    return -1;
	}
#if defined(APTAR0331_LIN_CMOS)	
    memcpy(p_coms_isp_default, &st_coms_isp_default_lin, sizeof(cmos_isp_default_t));
#elif defined(APTAR0331_WDR_CMOS)
    memcpy(p_coms_isp_default, &st_coms_isp_default_wdr, sizeof(cmos_isp_default_t));
#endif

    return 0;
}

void setup_sensor(int isp_mode)
{
	if(0 == isp_mode) /* ISP 'normal' isp_mode */
	{
        sensor_write_register(0x300C, 0x044C);
	}
	else if(1 == isp_mode) /* ISP pixel calibration isp_mode */
	{
		sensor_write_register(0x300C, 0x19C8);
		sensor_write_register(0x3012, 0x0460);
		sensor_write_register(0x3060, 0x1300);  //AG
		sensor_write_register(0x305E, 0x0080);  //DG
	}
}

HI_U32 cmos_get_sensor_mode(void)
{
#if defined(APTAR0331_LIN_CMOS)	
        return isp_special_alg_ar0331_lin;
#elif defined(APTAR0331_WDR_CMOS)
        return isp_special_alg_ar0331_wdr;
#endif
}

/****************************************************************************
 * callback structure                                                       *
 ****************************************************************************/

static SENSOR_EXP_FUNC_S stSensorExpFuncs = 
{
    .pfn_cmos_inttime_initialize = cmos_inttime_initialize,
    .pfn_cmos_inttime_update = cmos_inttime_update,

    .pfn_cmos_gains_initialize = cmos_gains_initialize,
    .pfn_cmos_gains_update = cmos_gains_update,
    .pfn_cmos_gains_update2 = NULL,
    .pfn_analog_gain_from_exposure_calculate = analog_gain_from_exposure_calculate,
    .pfn_digital_gain_from_exposure_calculate = digital_gain_from_exposure_calculate,

    .pfn_cmos_fps_set = cmos_fps_set,
    .pfn_vblanking_calculate = vblanking_calculate,
    .pfn_cmos_vblanking_front_update = cmos_vblanking_update,

    .pfn_setup_sensor = setup_sensor,

	.pfn_cmos_get_analog_gain = cmos_get_analog_gain,
	.pfn_cmos_get_digital_gain = cmos_get_digital_gain,
	.pfn_cmos_get_digital_fine_gain = cmos_get_digital_fine_gain,
    .pfn_cmos_get_iso = cmos_get_ISO,

	.pfn_cmos_get_isp_default = cmos_get_isp_default,
	.pfn_cmos_get_isp_special_alg = cmos_get_sensor_mode,
	
};

int sensor_register_callback(void)
{
	int ret;
	ret = HI_MPI_ISP_SensorRegCallBack(&stSensorExpFuncs);
	if (ret)
	{
	    printf("sensor register callback function failed!\n");
	    return ret;
	}
	return 0;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */



#endif // __AR0331_CMOS_H_
