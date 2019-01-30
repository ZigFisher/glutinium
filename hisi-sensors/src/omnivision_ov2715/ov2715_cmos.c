#if !defined(__OV2715_CMOS_H_)
#define __OV2715_CMOS_H_

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
	{   0x019D, 0x801F, 0x807E,
		0x8047, 0x018E, 0x8048,
		0x8016, 0x80AA, 0x01C0
	},

	// black level
	{0x40,0x40,0x40,0x40},

    //calibration reference color temperature 
    6500,

    //WB gain at 6500K, must keep consistent with calibration color temperature 
	{0x100 + 0x66, 0x100 + 0x00, 0x100 + 0x00, 0x100 + 0x54},

    // WB curve parameters, must keep consistent with reference color temperature.
	{0x1D, 0x9E, -0x46, 0x2862C, 0x80, -0x1F07B},

	// hist_thresh
	{0x20,0x60,0xc0,0xf0},
	
	0x0,	// iridix_balck
	0x1,	// rggb

	// gain
	0x1,	0x20,

	// iridix
	0x04,	0x08,	0xa0, 	0x4ff,
	
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
	cmos_inttime.full_lines_std = 1104;
	cmos_inttime.full_lines_std_30fps = 1104;
	cmos_inttime.full_lines = 1104;
	cmos_inttime.full_lines_limit = 65535;
	cmos_inttime.max_lines_target = 1104;
	cmos_inttime.min_lines_target = 8;
	cmos_inttime.vblanking_lines = 0;

	cmos_inttime.exposure_ashort = 0;
	cmos_inttime.exposure_shift = 0;

	cmos_inttime.lines_per_500ms = cmos_inttime.full_lines_std_30fps * 30 / 2; // 500ms / 39.17us
	cmos_inttime.flicker_freq = 0;//60*256;//50*256;

	return &cmos_inttime;
}

/*
 * This function applies the new integration time to the ISP registers.
 */
static __inline void cmos_inttime_update(cmos_inttime_ptr_t p_inttime) 
{
	HI_U32 _curr = p_inttime->exposure_ashort *16;

    //refresh the sensor setting every frame to avoid defect pixel error 
//	sensor_write_register(0x3212, 0x00);
	sensor_write_register(0x3502, _curr&0xFF);
	sensor_write_register(0x3501, (_curr>>8)&0xFF);
	sensor_write_register(0x3500, (_curr>>16)&0xFF);
	sensor_write_register(0x3212, 0x10);
	sensor_write_register(0x3212, 0xA0);
	
}

/*
 * This function applies the new vert blanking porch to the ISP registers.
 */
static __inline void cmos_vblanking_update(cmos_inttime_const_ptr_t p_inttime)
{
//	sensor_write_register(0x3212, 0x00);
	sensor_write_register(0x350C, p_inttime->vblanking_lines >> 8);
	sensor_write_register(0x350D, p_inttime->vblanking_lines & 0xff);
//	sensor_write_register(0x3212, 0x10);
//	sensor_write_register(0x3212, 0xA0);
}

static __inline HI_U16 vblanking_calculate(
		cmos_inttime_ptr_t p_inttime)
{
	if(p_inttime->exposure_ashort >= p_inttime->full_lines - 8)
	{
		p_inttime->exposure_ashort = p_inttime->full_lines - 8;
	}

	p_inttime->vblanking_lines = p_inttime->full_lines - p_inttime->full_lines_std_30fps;

	return p_inttime->exposure_ashort;
}

/* Set fps base */
static __inline void cmos_fps_set(
		cmos_inttime_ptr_t p_inttime,
		const HI_U8 fps
		)
{
	p_inttime->full_lines_std = p_inttime->full_lines_std_30fps * 30 / fps;

	return;
}

/*
 * This function initialises an instance of cmos_gains_t.
 */
static __inline cmos_gains_ptr_t cmos_gains_initialize()
{
	cmos_gains.max_again = 1;
	cmos_gains.max_dgain = 1024;

	cmos_gains.again_shift = 0;
	cmos_gains.dgain_shift = 4;
	cmos_gains.dgain_fine_shift = 0;


	return &cmos_gains;
}

#define	DGAIN_FIXED_POINT	4
#define	DGAIN_BITS_COUNT	32
static __inline unsigned int gain_to_reg(unsigned int dgain)
{
	int i;
	for(i=(DGAIN_BITS_COUNT-1);i>=DGAIN_FIXED_POINT;i--)
	{
		if((dgain&(1<<i))!=0)
		{
			break;
		}
	}
	return ((1<<(i-(DGAIN_FIXED_POINT-4)))-(1<<4))|((dgain>>(i-4))&0xF);
}
/*
 * This function applies the new gains to the ISP registers.
 */
static __inline void cmos_gains_update(cmos_gains_const_ptr_t p_gains)
{
	unsigned int dg = p_gains->dgain;
	unsigned int _g;

    //refresh the sensor setting every frame to avoid defect pixel error 
	_g = gain_to_reg(dg); 
//printf("\n_g:%#x\n", _g);
	sensor_write_register(0x3212, 0x00);
	sensor_write_register(0x350A, ((_g&0x100)>>8));
	sensor_write_register(0x350B, (_g&0xFF));
//	sensor_write_register(0x3212, 0x10);
//	sensor_write_register(0x3212, 0xA0);
	return;

}


static __inline HI_U32 analog_gain_from_exposure_calculate(
		cmos_gains_ptr_t p_gains,
		HI_U32 exposure,
		HI_U32 exposure_max,
		HI_U32 exposure_shift)
{
	p_gains->again = 0;
	   
	return exposure;
}

static __inline HI_U32 digital_gain_from_exposure_calculate(
		cmos_gains_ptr_t p_gains,
		HI_U32 exposure,
		HI_U32 exposure_max,
		HI_U32 exposure_shift)
{
	HI_U32 _dgain = (1<<p_gains->dgain_shift);
	int shft = 0;

	while (exposure > (1<<20))
	{
		exposure >>= 1;
		exposure_max >>= 1;
		++shft;
	}

	if (exposure > exposure_max)
	{
		//when setting manual exposure line, exposure_max>>shift should not be 0.
		exposure_max = DIV_0_TO_1(exposure_max);
		_dgain = (exposure  * _dgain + (exposure_max >> 1)) /exposure_max;
		exposure = exposure_max;
	}

	_dgain = _dgain < (1<<p_gains->dgain_shift) ? (1<<p_gains->dgain_shift): _dgain;
	_dgain = _dgain > p_gains->max_dgain ? p_gains->max_dgain : _dgain;

	p_gains->dgain = _dgain;
	return exposure << shft;
}

static void setup_sensor(int isp_mode)
{
	if(0 == isp_mode) /* ISP 'normal' isp_mode */
	{
	}
	else if(1 == isp_mode) /* ISP pixel calibration isp_mode */
	{
		sensor_write_register(0x350C, 0x15);
		sensor_write_register(0x350D, 0x90);
		sensor_write_register(0x3500, 0x01);  
		sensor_write_register(0x3501, 0x80);
		sensor_write_register(0x3502, 0x00);
		sensor_write_register(0x350A, 0x00);
		sensor_write_register(0x350B, 0x00);
	}
}

static HI_U32 cmos_gains_lin_to_db_convert(HI_U32 data, HI_U32 shift_in)
{
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
    return 0;
}

static HI_U8 cmos_get_digital_gain(cmos_gains_ptr_t p_gains)
{
    return cmos_gains_lin_to_db_convert(p_gains->dgain-(1<<p_gains->dgain_shift), p_gains->dgain_shift);
}

static HI_U8 cmos_get_digital_fine_gain(cmos_gains_ptr_t p_gains)
{
    return 0;
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

SENSOR_EXP_FUNC_S stSensorExpFuncs = 
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


#endif // __OV2175_CMOS_H_
