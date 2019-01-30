#if !defined(__AS3372_CMOS_H_)
#define __AS3372_CMOS_H_

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


#define EXPOSURE_LONG_INDEX             (0)
#define EXPOSURE_SHORT_INDEX            (1)
#define VBLANK_FRONT_PORCH_OFFSET_INDEX (2)
#define COL_BUF_GAIN_EVEN_EAST_INDEX    (3)
#define COL_BUF_GAIN_EVEN_WEST_INDEX    (4)
#define COL_BUF_GAIN_ODD_EAST_INDEX     (5)
#define COL_BUF_GAIN_ODD_WEST_INDEX     (6)
#define DP_COARSE_INDEX                 (7)
#define DP_FINE_GAIN_FACTOR_E_INDEX     (8)
#define DP_FINE_GAIN_FACTOR_O_INDEX     (9)
#define DP_IN_OFFSET_E_INDEX            (10)
#define DP_IN_OFFSET_O_INDEX            (11)
#define DP_OUT_OFFSET_E_INDEX           (12)
#define DP_OUT_OFFSET_O_INDEX           (13)
#define DP_CORR_SMALL_PUSH_OFFSET_O_INDEX (14)

#define EXPOSURE_LONG_ADDR             (0x0802)
#define EXPOSURE_SHORT_ADDR            (0x0803)
#define VBLANK_FRONT_PORCH_OFFSET_ADDR (0x0815)
#define COL_BUF_GAIN_EVEN_EAST_ADDR    (0x0822)
#define COL_BUF_GAIN_EVEN_WEST_ADDR    (0x0823)
#define COL_BUF_GAIN_ODD_EAST_ADDR     (0x0824)
#define COL_BUF_GAIN_ODD_WEST_ADDR     (0x0825)
#define DP_COARSE_ADDR                 (0x0067)
#define DP_FINE_GAIN_FACTOR_E_ADDR     (0x0068)
#define DP_FINE_GAIN_FACTOR_O_ADDR     (0x0069)
#define DP_IN_OFFSET_E_ADDR            (0x0071)
#define DP_IN_OFFSET_O_ADDR            (0x0072)
#define DP_OUT_OFFSET_E_ADDR           (0x0071)
#define DP_OUT_OFFSET_O_ADDR           (0x0072)
#define DP_CORR_SMALL_PUSH_OFFSET_O_ADDR (0x006D)


/****************************************************************************
 * local variables															*
 ****************************************************************************/

static cmos_inttime_t cmos_inttime;
static cmos_gains_t cmos_gains;


static cmos_isp_default_t st_coms_isp_default = 
{
	// color matrix[9]
	{  0x01F0,  0x8160,  0x0069,
	    0x8040,  0x01C1,  0x8081,
	    0x8043,  0x80D0,  0x0213
	},

	// black level
	{0x40,0x40,0x40,0x40},

	//calibration reference color temperature 
	6500,

	//WB gain at 6500K, must keep consistent with calibration color temperature 
	{0x100 + 0xD8, 0x100 + 0x00, 0x100 + 0x00, 0x100 + 0x50},

	// WB curve parameters, must keep consistent with reference color temperature.
	{0x1D, 0x9E, -0x46, 0x2862C, 0x80, -0x1F07B},

	// hist_thresh
	{0x20,0x60,0xc0,0xf0},
	
	0x00,	// iridix_balck
	0x0,	// rggb

	// gain
	0x8,	0x10,

	// iridix
	0x04,	0x08,	0xa0, 	0x4ff,
	
	0x1, 	// balance_fe
	0x80,	// ae compensation
	0x18, 	// sinter threshold
	
    0x0,  0,  0  //noise profile 0, use the default noise profile lut, don't need to set nr0 and nr1
};


/*
 * This function initialises an instance of cmos_inttime_t.
 */
static __inline cmos_inttime_const_ptr_t cmos_inttime_initialize()
{
	cmos_inttime.full_lines_std = 1125;
	cmos_inttime.full_lines_std_30fps = 1125;
	cmos_inttime.full_lines = 1125;
	cmos_inttime.full_lines_del = 1125; //TODO: remove
	cmos_inttime.full_lines_limit = 65535;
	cmos_inttime.max_lines_target = 1123;
	cmos_inttime.min_lines_target = 2;
	cmos_inttime.vblanking_lines = 1125;

	cmos_inttime.exposure_ashort = 0;

	cmos_inttime.lines_per_500ms = 1125*30; 
	cmos_inttime.flicker_freq = 0;//60*256;//50*256;

	return &cmos_inttime;
}

/*
 * This function applies the new integration time to the ISP registers.
 */
static __inline void cmos_inttime_update(cmos_inttime_ptr_t p_inttime) 
{
	HI_U16 eshort = p_inttime->exposure_ashort;
	HI_U16 elong  = p_inttime->exposure_ashort;//?
	sensor_write_register(EXPOSURE_SHORT_ADDR, eshort);
	sensor_write_register(EXPOSURE_LONG_ADDR, elong);
}

/*
 * This function applies the new vert blanking porch to the ISP registers.
 */
static __inline void cmos_vblanking_update(cmos_inttime_const_ptr_t p_inttime)
{
	sensor_write_register(VBLANK_FRONT_PORCH_OFFSET_ADDR, p_inttime->vblanking_lines);
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
	p_inttime->vblanking_lines = p_inttime->full_lines_del - p_inttime->full_lines_std_30fps + 5;
	p_inttime->exposure_along  = p_inttime->full_lines_del - p_inttime->exposure_along;
	return p_inttime->full_lines_del - p_inttime->exposure_ashort;
}

/* Set fps base */
static __inline void cmos_fps_set(
		cmos_inttime_ptr_t p_inttime,
		const HI_U8 fps
		)
{
#ifdef DUAL_PORT
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
#else
	// the default framerate
	p_inttime->full_lines_std = (1125 * 30) / fps;
	sensor_write_register(0xb6, 0x09);
	p_inttime->lines_per_500ms = 1125 * fps / 2;
#endif
}

/*
 * This function initialises an instance of cmos_gains_t.
 */
static __inline cmos_gains_ptr_t cmos_gains_initialize()
{
	cmos_gains.max_again = 45;
	cmos_gains.max_dgain = 255;

	cmos_gains.again_shift = 4;
	cmos_gains.dgain_shift = 0;
	cmos_gains.dgain_fine_shift = 0;

	cmos_gains.dgain_offset = 1 << 12;
	cmos_gains.max_dgain_step = 4140; // (6.0142dB / 64) << 12

	return &cmos_gains;
}

static __inline HI_U16 analog_gain_lut_get_value(HI_U8 index)
{
	static HI_U16 gain_lut[] = {255, 583, 455, 511, 847, 1271, 1023, 1735, 1919, 2047};
	return gain_lut[index];
}

/*
 * This function applies the digital gain
 * input and output offset and correction
 */
static __inline HI_U16 cmos_gains_dpga_update(cmos_gains_const_ptr_t p_gains)
{
	HI_U16 again;
	HI_U16 dgain;
	HI_U16 _in_ofst;
	HI_U16 _out_ofst;
	HI_U16 _correction;
       HI_U16 _idx;

	static HI_U16 in_offsets_lut[] = 
			{92, 108, 112, 160, 160, 160, 160, 160, 160, 160, 160, 160, 160};

	static HI_U16 out_offsets_lut[] = 
			{64, 64,  64,  68,  70,  80,  112, 160, 224, 320, 448, 640, 896}; //would need to correct ISP offsets...
			//{64, 64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  64};

	static HI_U16 correction_lut[] = 
			{2,  4,   6,   8,   11,  11,  11,  11,  11,  11,  11,  11,  11};

	again = p_gains->again*1024;
	dgain = (p_gains->dgain*1024 + p_gains->dgain_fine)*6;

	_idx = (again + dgain)/3;
	if(_idx>12*1024) _idx=12*1024;

	_in_ofst = (in_offsets_lut[_idx/1024]*(1024+(_idx&0xfc00)-_idx) + in_offsets_lut[_idx/1024 + 1]*(_idx-(_idx&0xfc00)))/1024;
	_out_ofst = (out_offsets_lut[_idx/1024]*(1024+(_idx&0xfc00)-_idx) + out_offsets_lut[_idx/1024 + 1]*(_idx-(_idx&0xfc00)))/1024;
	_correction = correction_lut[_idx/1024];

//	_in_ofst = 160;
//	_out_ofst = 64;
//	_correction = 11;

	sensor_write_register(0x062, 0x8000+_in_ofst);
	sensor_write_register(0x071, 0x8000+_in_ofst);
	sensor_write_register(0x072, 0x8000+_in_ofst);
	sensor_write_register(0x075, 0x8000+_in_ofst);

//	// Update ISP with the new black offset
//	_out_ofst = _out_ofst>224 ? 224 : _out_ofst;

	sensor_write_register(0x076, 0x8000+_out_ofst);
	sensor_write_register(0x077, 0x8000+_out_ofst);

	sensor_write_register(0x06d, _correction);
	sensor_write_register(0x066, 0x80b0);

//	printf("%x - ", again/1024);
//	printf("%x - ", dgain/1024);
//	printf("%x - ", _idx/1024);
//	printf("%x - ", _in_ofst);
//	printf("%x - ", _out_ofst);
//	printf("%x\n", _correction);

//	if(again > 7)
//		_correction = 0x34;
//	else
//		_correction = 0x4c;
//
//	update_sensor_register(0x1915, _correction, 1000);

	return _out_ofst;
}

/*
 * This function applies the new gains to the ISP registers.
 */
static __inline HI_U16 cmos_gains_update2(cmos_gains_const_ptr_t p_gains)
{

	HI_U16 data16;
//	HI_U16 out_offset;
//	HI_U32 data32;
	HI_U16 lut_val;

	{
		lut_val = analog_gain_lut_get_value(p_gains->again);

		sensor_write_register(COL_BUF_GAIN_EVEN_EAST_ADDR, lut_val);
		sensor_write_register(COL_BUF_GAIN_EVEN_WEST_ADDR, lut_val);
		sensor_write_register(COL_BUF_GAIN_ODD_EAST_ADDR, lut_val);
		sensor_write_register(COL_BUF_GAIN_ODD_WEST_ADDR, lut_val);

		// Update the ext status	
		//apical_ext_exposure_status_analog_gain_status_write(lut_val);
	}

	{
		 data16 = p_gains->dgain_fine;
		// digital_gain_fine data - set bit15
		data16 = data16 | 0x8000;

		sensor_write_register(DP_FINE_GAIN_FACTOR_E_ADDR, data16);
		sensor_write_register(DP_FINE_GAIN_FACTOR_O_ADDR, data16);

		// Update the ext status
		//apical_ext_exposure_status_fine_digital_gain_status_write(data16);
	}

	{
		HI_U8 data_lut = p_gains->dgain + 4;
		if (data_lut > 15)
			data_lut = 15;

		data16 = (data_lut << 4) | data_lut;
		// digital_gain_coarse data - set bit15
		// digital_gain_coarse data - set bit8
		data16 = data16 | 0x8100;

		sensor_write_register(DP_COARSE_ADDR, data16);

		// Update the ext status
		//apical_ext_exposure_status_coarse_digital_gain_status_write(data16);
	}

	return cmos_gains_dpga_update(p_gains);
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
	int _i;
	HI_U32 _again = 0;
	HI_U32 exposure1;
	int shft = 0;
	// normalize
	while (exposure > (1<<22))
	{
		exposure >>= 1;
		exposure_max >>= 1;
		++shft;
	}
	for(_i = 1; _i <= 8; _i++)
	{
		exposure1 = (exposure*913) >> 10;
		if(exposure1 <= exposure_max)
			break;
		++_again;
		exposure = exposure1;
	}
	p_gains->again = _again;
	return exposure << shft;
}

static __inline HI_U32 digital_gain_from_exposure_calculate(
		cmos_gains_ptr_t p_gains,
		HI_U32 exposure,
		HI_U32 exposure_max,
		HI_U32 exposure_shift)
{
	int _i;
	HI_U32 _dgain = p_gains->dgain_offset;
	HI_U32 _ares = p_gains->dgain_offset;
	HI_U32 _lres = 0;
	int shft = 0;

	while (exposure > (1<<20))
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
	        // exposure >= exposure_shift
	        exposure = (exposure < exposure_shift) ? exposure_shift : exposure;
		_dgain = (_dgain * exposure) / (exposure / exposure_shift) / exposure_shift;
	}
	_dgain = _dgain < p_gains->dgain_offset ? p_gains->dgain_offset : _dgain;
	_dgain = _dgain > (HI_U32)p_gains->dgain_offset * p_gains->max_dgain_target ?
		(HI_U32)p_gains->dgain_offset * p_gains->max_dgain_target : _dgain;

	for(_i=0; _i<(64*4*4); _i++)
	{
		if(_ares >= _dgain)
			break;
		_ares = (_ares * p_gains->max_dgain_step) / p_gains->dgain_offset;
	}
	_lres = _i;
	//exposure = exposure * (gain_offset / 64) / (_ares / 64);
	_dgain      = _lres * 16;

	p_gains->dgain_fine	= _dgain & 0x03FF;
	p_gains->dgain		= _dgain >> 10;

	return exposure << shft;
}

static void setup_sensor(int isp_mode)
{
	if(0 == isp_mode) /* setup for ISP 'normal mode' */
	{
	}
	else if(1 == isp_mode) /* setup for ISP pixel calibration mode */
	{
		sensor_write_register(VBLANK_FRONT_PORCH_OFFSET_ADDR, 17908/2-1125+5);

		sensor_write_register(EXPOSURE_SHORT_ADDR, 2);

		sensor_write_register(EXPOSURE_LONG_ADDR, 2);

		sensor_write_register(COL_BUF_GAIN_EVEN_EAST_ADDR, 255);
		sensor_write_register(COL_BUF_GAIN_EVEN_WEST_ADDR, 255);
		sensor_write_register(COL_BUF_GAIN_ODD_EAST_ADDR, 255);
		sensor_write_register(COL_BUF_GAIN_ODD_WEST_ADDR, 255);

		sensor_write_register(0x062, 0x8000+92);
		sensor_write_register(0x071, 0x8000+92);
		sensor_write_register(0x072, 0x8000+92);
		sensor_write_register(0x075, 0x8000+92);

		sensor_write_register(0x076, 0x8000+64);
		sensor_write_register(0x077, 0x8000+64);

		sensor_write_register(0x06d, 2);
		sensor_write_register(0x066, 0x80b0);

		{
			HI_U16 data16 = 0;
			// digital_gain_fine data - set bit15
			data16 = data16 | 0x8000;
			sensor_write_register(DP_FINE_GAIN_FACTOR_O_ADDR, data16);

			data16 = 0;
			// digital_gain_fine data - set bit15
			data16 = data16 | 0x8000;
			sensor_write_register(DP_FINE_GAIN_FACTOR_E_ADDR, data16);
		}

		{
			HI_U8 data_lut = 4;

			HI_U16 data16 = (data_lut << 4) | data_lut;
			// digital_gain_coarse data - set bit15
			// digital_gain_coarse data - set bit8
			data16 = data16 | 0x8100;
			sensor_write_register(DP_COARSE_ADDR, data16);
		}
	}
}

static HI_U8 cmos_get_analog_gain(cmos_gains_ptr_t cmos_gains)
{
    return cmos_gains->again;
}

static HI_U8 cmos_get_digital_gain(cmos_gains_ptr_t cmos_gains)
{
    return cmos_gains->dgain;
}

static HI_U8 cmos_get_digital_fine_gain(cmos_gains_ptr_t cmos_gains)
{
    return cmos_gains->dgain_fine;
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

HI_U32 cmos_get_isp_speical_alg(void)
{
    return isp_special_alg_0;
}


/****************************************************************************
 * callback structure                                                       *
 ****************************************************************************/

SENSOR_EXP_FUNC_S stSensorExpFuncs = 
{
    .pfn_cmos_inttime_initialize = cmos_inttime_initialize,
    .pfn_cmos_inttime_update = cmos_inttime_update,

    .pfn_cmos_gains_initialize = cmos_gains_initialize,
    .pfn_cmos_gains_update = NULL,
    .pfn_cmos_gains_update2 = cmos_gains_update2,
    .pfn_analog_gain_from_exposure_calculate = analog_gain_from_exposure_calculate,
    .pfn_digital_gain_from_exposure_calculate = digital_gain_from_exposure_calculate,

    .pfn_cmos_fps_set = cmos_fps_set,
    .pfn_vblanking_calculate = vblanking_calculate,
    .pfn_cmos_vblanking_front_update = cmos_vblanking_update,

    .pfn_setup_sensor = setup_sensor,

	.pfn_cmos_get_analog_gain = cmos_get_analog_gain,
	.pfn_cmos_get_digital_gain = cmos_get_digital_gain,
	.pfn_cmos_get_digital_fine_gain = cmos_get_digital_fine_gain,
    .pfn_cmos_get_iso = NULL,

	.pfn_cmos_get_isp_default = cmos_get_isp_default,
	.pfn_cmos_get_isp_special_alg = cmos_get_isp_speical_alg,
	
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


#endif // __AS3372_CMOS_H_
