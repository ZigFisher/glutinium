#if !defined(__APTINA_CMOS_H_)
#define __APTINA_CMOS_H_

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


static cmos_isp_default_t st_coms_isp_default = 
{
	// color matrix[9]
	{	415,      32768+73, 32768+76,
		32768+74, 409,      32768+75,
		32768+68, 32768+73, 400
	},

	// black level
	{0xA8,0xA8,0xA8,0xA8},

    //calibration reference color temperature 
    6500,

    //WB gain at 6500K, must keep consistent with calibration color temperature 
	{0x100 + 0x5f, 0x100 + 0x00, 0x100 + 0x00, 0x100 + 0xc8},

    // WB curve parameters, must keep consistent with reference color temperature.
	{0x1D, 0x9E, -0x46, 0x2862C, 0x80, -0x1F07B},

	// hist_thresh
	{0x20,0x60,0xc0,0xf0},
	
	0x0B,	// iridix_balck
	0x1,	// rggb

	// gain
	0x8,	0x8,

	// iridix
	0x04,	0x01,	0x30, 	0x4ff,
	
	0x1, 	// balance_fe
	0x80,	// ae compensation
	0x15, 	// sinter threshold

    0x0,  0,  0  //noise profile=0, use the default noise profile lut, don't need to set nr0 and nr1
	
};


/*
 * This function initialises an instance of cmos_inttime_t.
 */
static __inline cmos_inttime_const_ptr_t cmos_inttime_initialize()
{
	cmos_inttime.full_lines_std = 1534;
	cmos_inttime.full_lines_std_30fps = 1535;
	cmos_inttime.full_lines = 1534;
	cmos_inttime.full_lines_del = 1534; //TODO: remove
	cmos_inttime.full_lines_limit = 65535;
	cmos_inttime.max_lines_target = 1534;
	cmos_inttime.min_lines_target = 2;
	cmos_inttime.vblanking_lines = 1534;

	cmos_inttime.exposure_ashort = 0;
	cmos_inttime.exposure_shift = 0;

	cmos_inttime.lines_per_500ms = 19216; // 500ms / 26.02us
	cmos_inttime.flicker_freq = 0;//60*256;//50*256;

	return &cmos_inttime;
}

/*
 * This function applies the new integration time to the ISP registers.
 */
static __inline void cmos_inttime_update(cmos_inttime_ptr_t p_inttime) 
{
	HI_U16 _time = p_inttime->exposure_ashort >> p_inttime->exposure_shift;

	/*	
		To avoid frame rate jitter, shutter width should under acl+vb according to datasheet.
		acl + vb = 1079 + 8 = 1088
		
		<added by xujilin, Dec 03>
	*/
	if (_time >= 1088)
	{
	#ifdef PRINT_DEBUG_INFO
		printf("---_time = %d\n", _time);
	#endif
		_time = 1088;
	}
	
	sensor_write_register(0x09, _time);
}

/*
 * This function applies the new vert blanking porch to the ISP registers.
 */
static __inline void cmos_vblanking_front_update(cmos_inttime_const_ptr_t p_inttime)
{
}
static __inline void inttime_from_exposure_calculate(
		cmos_inttime_ptr_t p_inttime,
		HI_U32 exposure, 
		HI_U8 shift)
{
}

static __inline void antiflicker_correct_inttime(
		cmos_inttime_ptr_t p_inttime)
{
}

static __inline HI_U16 vblanking_calculate(
		cmos_inttime_ptr_t p_inttime)
{
	//
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
//	alt_printf("full_lines_del = %x\n", p_inttime->full_lines_del);
#endif
	p_inttime->vblanking_lines = p_inttime->full_lines_del - 1080;
#if defined(TRACE_ALL)
//	alt_printf("vblanking_lines = %x\n", p_inttime->vblanking_lines);
#endif
	return p_inttime->exposure_ashort;
}

/* Set fps base */
static __inline void cmos_fps_set(
		cmos_inttime_ptr_t p_inttime,
		const HI_U8 fps
		)
{
#if 0	// This section is not ready for Aptina currently.
	switch(fps)
	{
		default:
			// Change the frame rate via changing the vertical blanking
			p_inttime->full_lines_std = (1125 * 60) / fps;
			sensor_write_register(0xb6, 0x09);
			// These modes are configured via vertical blanking, thus 'lines_per_500ms' stays fixed
			p_inttime->lines_per_500ms = 1125 * 30;
		break;
		case 50:
			// Change the frame rate via changing the horizontal blanking
			p_inttime->full_lines_std = 1125;
			sensor_write_register(0xb6, 0x40);
			p_inttime->lines_per_500ms = 1125 * 25;
		break;
		case 25:
			// Change the frame rate via changing both vertical and horizontal blanking
			p_inttime->full_lines_std = 1125 * 2;
			sensor_write_register(0xb6, 0x40);
			p_inttime->lines_per_500ms = 1125 * 25;
		break;
	}
#endif	
}

/*
 * This function initialises an instance of cmos_gains_t.
 */
static __inline cmos_gains_ptr_t cmos_gains_initialize()
{
	cmos_gains.max_again = 64;
	cmos_gains.max_dgain = 128;

	cmos_gains.again_shift = 3;
	cmos_gains.dgain_shift = 3;
	cmos_gains.dgain_fine_shift = 0;
	
	return &cmos_gains;
}

/*
 * This function applies the new gains to the ISP registers.
 */
static __inline void cmos_gains_update(cmos_gains_const_ptr_t p_gains)
{
	int ag = p_gains->again;
	int dg = p_gains->dgain;
	int gain = ((dg-8)<<8) + (ag > 32 ? ag = 0x40 + (ag>>1) : ag);

	/*	
		To avoid sensor no FV, problem may caused by 0x07's sync bit.
		currently, use global gain instead of 4-gains.

		<added by xujilin, Dec 02>
	*/

#if 0
	sensor_write_register(0x07, 0x1f8a + 0x1);
	sensor_write_register(0x2b, gain);
	sensor_write_register(0x2c, gain);
	sensor_write_register(0x2d, gain);
	sensor_write_register(0x2e, gain);
	sensor_write_register(0x07, 0x1f8a);
#else
	// use global gain
	sensor_write_register(0x35, gain);
#endif
}

#if 0
/* Emulate digital fine gain */
static __inline void em_dgain_fine_update(cmos_gains_ptr_t p_gains)
{
	HI_U32 gain;
	gain = ((0x15f * p_gains->dgain_fine)) / 4096;
	apical_isp_white_balance_gain_00_write(gain);
	gain = ((0x100 * p_gains->dgain_fine)) / 4096;
	apical_isp_white_balance_gain_01_write(gain);
	gain = ((0x100 * p_gains->dgain_fine)) / 4096;
	apical_isp_white_balance_gain_10_write(gain);
	gain = ((0x1c8 * p_gains->dgain_fine)) / 4096;
	apical_isp_white_balance_gain_11_write(gain);
}
#endif

static __inline HI_U32 analog_gain_from_exposure_calculate(
		cmos_gains_ptr_t p_gains,
		HI_U32 exposure,
		HI_U32 exposure_max,
		HI_U32 exposure_shift)
{
//	int _i;
	HI_U32 _again = 8;
	HI_U32 exposure1;
	int shft = 0;

	// normalize
	while (exposure > (1<<22))
	{
		exposure >>= 1;
		exposure_max >>= 1;
		++shft;
	}
	//max_again = boundary_max_check(max_again * 8, 126);
	//when setting manual exposure line, exposure_max>>shift should not be 0.
       exposure_max = DIV_0_TO_1(exposure_max);
	_again = ((exposure<<3)+(exposure_max>>1))/exposure_max;
	_again = _again < 8 ? 8 : _again;
	_again = _again > p_gains->max_again_target ? p_gains->max_again_target : _again;
	if(_again > 63) // model x2 bit
	{
		_again = 63;
	}

	if(exposure >= exposure_max)
	{
		exposure1 = (((exposure<<3) + (_again>>1)) / _again) << shft;
	}
	else
	{
		exposure1 = exposure;
		_again = 8;
	}
	p_gains->again = _again;
	return exposure1;
}

static __inline HI_U32 digital_gain_from_exposure_calculate(
		cmos_gains_ptr_t p_gains,
		HI_U32 exposure,
		HI_U32 exposure_max,
		HI_U32 exposure_shift)
{
//	int _i;
	HI_U32 _dgain = 8;
	HI_U32 _dgain_fine = 4096;
//	HI_U32 _dgain_step = 1 + (8 << 8);
//	HI_U32 _ares = 8 << 8;
	HI_U32 exposure1;
	int shft = 0;

//	max_dgain = boundary_max_check(max_dgain * 8, 120);

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
		_dgain = ((exposure<<3)+(exposure_max>>1))/exposure_max,
		_dgain = _dgain < 8 ? 8 :_dgain;
		_dgain = _dgain > p_gains->max_dgain_target ? p_gains->max_dgain_target : _dgain; 
//		_dgain = ((exposure  * _dgain) / exposure_shift) / (exposure_max/exposure_shift);
//		_dgain = ((exposure  * _dgain) + (exposure_max>>1)) / (exposure_max);
//		if(_dgain > 63) // model x2 bit
//		{
//			_dgain = 63;
//		}

		//exposure1 = (((exposure<<3) + (_dgain>>1)) / _dgain) << shft;
		exposure1 = exposure_max;
//		*dgain = boundaries_check(_dgain, 8, max_dgain);
		p_gains->dgain = _dgain;
	}
	else
	{
	        // exposure >= exposure_shift
	        exposure = (exposure < exposure_shift) ? exposure_shift : exposure;
		_dgain_fine = (_dgain_fine * exposure) / (exposure / exposure_shift) / exposure_shift;
		_dgain = 8;
		exposure1 = exposure;
	}

	p_gains->dgain = _dgain;
				
	p_gains->dgain_fine = _dgain_fine;

	return exposure1;
}

static void setup_sensor(int isp_mode)
{
	if(0 == isp_mode) /* ISP 'normal' isp_mode */
	{
	}
	else if(1 == isp_mode) /* ISP pixel calibration isp_mode */
	{
		//	sensor_write_register(0x06, 4031); // 5fps

		sensor_write_register(0x09, 4000);

		sensor_write_register(0x2b, 8);
		sensor_write_register(0x2c, 8);
		sensor_write_register(0x2d, 8);
		sensor_write_register(0x2e, 8);
	}
}

static HI_U32 cmos_gains_lin_to_db_convert(HI_U32 data, HI_U32 shift_in)
{
//	HI_U32 _i;
	HI_U32 _res = 0;
	if(0 == data)
		return _res;

	for(;;)
	{
		data = (data*913 + (1<<9)) >> 10;
		if(data < (1<<shift_in))
			break;
		++_res;
	}
	return _res;
}

static HI_U8 cmos_get_analog_gain(cmos_gains_ptr_t p_gains)
{
    return cmos_gains_lin_to_db_convert(p_gains->again, p_gains->again_shift);
}

static HI_U8 cmos_get_digital_gain(cmos_gains_ptr_t p_gains)
{
    return cmos_gains_lin_to_db_convert(p_gains->dgain-(1<<p_gains->dgain_shift), p_gains->dgain_shift);
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
    memcpy(p_coms_isp_default, &st_coms_isp_default, sizeof(cmos_isp_default_t));

    return 0;
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
    .pfn_cmos_vblanking_front_update = cmos_vblanking_front_update,

    .pfn_setup_sensor = setup_sensor,

	.pfn_cmos_get_analog_gain = cmos_get_analog_gain,
	.pfn_cmos_get_digital_gain = cmos_get_digital_gain,
	.pfn_cmos_get_digital_fine_gain = cmos_get_digital_fine_gain,
    .pfn_cmos_get_iso = NULL,

	.pfn_cmos_get_isp_default = cmos_get_isp_default,
	.pfn_cmos_get_isp_special_alg = NULL,
	
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


#endif // __APTINA_CMOS_H_
