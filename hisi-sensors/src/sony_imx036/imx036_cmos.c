#if !defined(__IMX036_CMOS_H_)
#define __IMX036_CMOS_H_

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "hi_comm_sns.h"
#include "hi_sns_ctrl.h"
#include "mpi_isp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/* Note: format of address is special.
 * chip_id + reg_adddr */
#define EXPOSURE_ADDR (0x20C) //2:chip_id, 0C: reg addr.
#define APGC_ADDR (0x205)
#define DPGC_ADDR (0x261)
#define VMAX_ADDR (0x209)

/****************************************************************************
 * local variables															*
 ****************************************************************************/

static cmos_inttime_t cmos_inttime;
static cmos_gains_t cmos_gains;

static cmos_isp_default_t st_coms_isp_default = 
{
    // color correction matrix
	{
		0x0196, 0x8076, 0x8020,
		0x8017, 0x0190, 0x8079,
		0x000f, 0x8056, 0x0147,
	},

	// black level for R, Gr, Gb, B channels
	{0xF0,0xF0,0xF0,0xF0},

	//calibration reference color temperature 
	5000,

	//WB gain at 5000K, must keep consistent with calibration color temperature 
	{0x166, 0x100, 0x100, 0x1B7},

	// WB curve parameters, must keep consistent with reference color temperature.
	{-6714, 15009, 8039, 175202, 0x80, -113875},
 		
	// hist_thresh
	{0x20,0x60,0xc0,0xf0},
	
	0x00,	// iridix_balck
	0x0,	// 0:rggb; 2: gbrg 

	// max again, max dgain 
	0x10,	0x8,

	// iridix
	0x04,	0x08,	0xa0, 	0x8ff,
	
	0x1, 	// balance_fe
	0x80,	// ae compensation
	0x8, 	// sinter threshold

	0x1,  //0: use default profile table; 1: use calibrated profile lut, the setting for nr0 and nr1 must be correct.
	0,    //nr0
	639   //nr1 
};

/*
 * This function initialises an instance of cmos_inttime_t.
 */
static __inline cmos_inttime_const_ptr_t cmos_inttime_initialize()
{
	cmos_inttime.full_lines_std = 1125;
	cmos_inttime.full_lines_std_30fps = 1125;
	cmos_inttime.full_lines = 1125;
	cmos_inttime.full_lines_limit = 65535;
	cmos_inttime.max_lines_target = 1123;
	cmos_inttime.min_lines_target = 2;
	cmos_inttime.vblanking_lines = 1125;

	cmos_inttime.exposure_ashort = 0;

	cmos_inttime.lines_per_500ms = 16874; // 500ms / 29.63us = 16874
	cmos_inttime.flicker_freq = 0;//60*256;//50*256;

	return &cmos_inttime;
}

/*
 * This function applies the new integration time to the ISP registers.
 */
static __inline void cmos_inttime_update(cmos_inttime_ptr_t p_inttime) 
{
    HI_U16 exp_time;

    exp_time = p_inttime->full_lines - p_inttime->exposure_ashort;

    sensor_write_register(EXPOSURE_ADDR, exp_time & 0xFF);
    sensor_write_register(EXPOSURE_ADDR + 1, (exp_time & 0xFF00) >> 8);
}

/*
 * This function applies the new vert blanking porch to the ISP registers.
 */
static __inline void cmos_vblanking_update(cmos_inttime_const_ptr_t p_inttime)
{
	HI_U16 vmax = p_inttime->full_lines;
	sensor_write_register(VMAX_ADDR, (vmax&0x00ff));
	sensor_write_register(VMAX_ADDR+1, ((vmax&0xff00) >> 8));
	
	return;
}

static __inline HI_U16 vblanking_calculate(
		cmos_inttime_ptr_t p_inttime)
{
	if (p_inttime->exposure_ashort >= p_inttime->full_lines - 3)
	{
		p_inttime->exposure_ashort = p_inttime->full_lines - 3;
	}

	p_inttime->vblanking_lines = p_inttime->full_lines - p_inttime->full_lines_std;

	return p_inttime->exposure_ashort;
}


/* Set fps base */
static __inline void cmos_fps_set(
		cmos_inttime_ptr_t p_inttime,
		const HI_U8 fps
		)
{
	switch(fps)
	{
		case 30:
			// Change the frame rate via changing the vertical blanking
			p_inttime->full_lines_std = 1125;
			sensor_write_register(VMAX_ADDR, 0x65);
			sensor_write_register(VMAX_ADDR+1, 0x04);
			p_inttime->lines_per_500ms = 1125 * 30 / 2;
		break;
		
		case 25:
			// Change the frame rate via changing the vertical blanking
			p_inttime->full_lines_std = 1350;
			sensor_write_register(VMAX_ADDR, 0x46);
			sensor_write_register(VMAX_ADDR+1, 0x05);
			p_inttime->lines_per_500ms = 1350 * 25 / 2;
		break;
		
		default:
		break;
	}
}

/*
 * This function initialises an instance of cmos_gains_t.
 */
static __inline cmos_gains_ptr_t cmos_gains_initialize()
{
	cmos_gains.again_shift = 4;
	cmos_gains.dgain_shift = 4;
	cmos_gains.dgain_fine_shift = 0;
    
	cmos_gains.max_again = 16 << cmos_gains.again_shift;  //linear
	cmos_gains.max_dgain = 8 << cmos_gains.dgain_shift; //linear

	return &cmos_gains;
}

static __inline HI_U16 analog_gain_lut_get_value(HI_U8 index)
{
    static HI_U16 gain_lut[] = 
    {
        0x000, 0x023, 0x044, 0x065, 0x084, 0x0A2, 0x0C0, 0x0DC, 0x0F7, 0x112,
        0x12B, 0x144, 0x15B, 0x172, 0x189, 0x19E, 0x1B3, 0x1C7, 0x1DA, 0x1ED,
        0x1FF, 0x210, 0x221, 0x231, 0x241, 0x250, 0x25F, 0x26D, 0x27B, 0x288,
        0x295, 0x2A1, 0x2AD, 0x2B8, 0x2C4, 0x2CE, 0x2D9, 0x2E3, 0x2EC, 0x2F6,
        0x2FF, 0x308, 0x310, 0x318, 0x320, 0x328, 0x32F, 0x336, 0x33D, 0x344,
        0x34A, 0x350, 0x356, 0x35C, 0x361, 0x367, 0x36C, 0x371, 0x376, 0x37B,
        0x37F, 0x383, 0x388, 0x38C, 0x390, 0x394, 0x397, 0x39B, 0x39E, 0x3A2,
        0x3A5, 0x3A8, 0x3AB, 0x3AE, 0x3B1, 0x3B3, 0x3B6, 0x3B8, 0x3BB, 0x3BD,
        0x3BF
    };
	return gain_lut[index];
}


/*
 * This function applies the new gains to the ISP registers.
 */
static __inline void cmos_gains_update(cmos_gains_const_ptr_t p_gains)
{

	// HI_U16 data16;
	HI_U16 lut_val;

    /* analog gain */
	{
        assert(p_gains->again_db <= 80);
		lut_val = analog_gain_lut_get_value(p_gains->again_db);
		sensor_write_register(APGC_ADDR, lut_val & 0xFF);
		sensor_write_register(APGC_ADDR + 1, (lut_val & 0x0300) >> 8);        
	}

    /* digital gain. 
      * digital_gain = DPGC * 0.3 db.
      * DPCG  = [00h, 3Ch];
      * digital_gain = [0db, 18db]. */
	{
        assert(p_gains->dgain_db <= 0x3C);
		sensor_write_register(DPGC_ADDR, p_gains->dgain_db);
	}
	return;
}


static __inline HI_U32 analog_gain_from_exposure_calculate(
		cmos_gains_ptr_t p_gains,
		HI_U32 exposure,
		HI_U32 exposure_max,
		HI_U32 exposure_shift)
{
	int _i;
	HI_U32 _again = 0;
	HI_U32 exposure0, exposure1;
	int shft = 0;
	// normalize

	while (exposure > (1<<22))
	{
		exposure >>= 1;
		exposure_max >>= 1;
		++shft;
	}

    exposure0 = exposure;
       /* again unit: 0.3db */
	for(_i = 1; _i <= 80; _i++)
	{
		exposure1 = (exposure0*989) >> 10;
		if(exposure1 <= exposure_max)
			break;
		++_again;
		exposure0 = exposure1;
	}
	p_gains->again = (exposure << p_gains->again_shift) / exposure0; 
	p_gains->again_db = _again;
	return exposure0 << shft;
}

static __inline HI_U32 digital_gain_from_exposure_calculate(
		cmos_gains_ptr_t p_gains,
		HI_U32 exposure,
		HI_U32 exposure_max,
		HI_U32 exposure_shift)
{
	int _i;
	HI_U32 _dgain = 0;
	HI_U32 exposure0, exposure1;
	int shft = 0;
	// normalize
	while (exposure > (1<<20)) /* analog use (1<<22) for analog exposure is bigger. */
	{
		exposure >>= 1;
		exposure_max >>= 1;
		++shft;
	}

	exposure0 = exposure;

	/* unit: 0.3db */
	for(_i = 1; _i <= 0x3C; _i++)
	{
		exposure1 = (exposure0*989) >> 10;
		if(exposure1 <= exposure_max)
			break;
		++_dgain;
		exposure0 = exposure1;
	}
	p_gains->dgain = (exposure << p_gains->dgain_shift) / exposure0; 
       p_gains->dgain_db = _dgain; 	
	return exposure0 << shft;
}

static void setup_sensor(int isp_mode)
{
	if(0 == isp_mode) /* setup for ISP 'normal mode' */
	{
        sensor_write_register(VMAX_ADDR, 0x65);
        sensor_write_register(VMAX_ADDR + 1, 0x04);
	}
	else if(1 == isp_mode) /* setup for ISP pixel calibration mode */
	{
        //TODO: finish this.
        /* Sensor must be programmed for slow frame rate (5 fps and below)*/
        /* change frame rate to 5 fps by setting 1 frame length = 1125 * (30/5) */
        sensor_write_register(VMAX_ADDR, 0x5E);
        sensor_write_register(VMAX_ADDR + 1, 0x1A);

        /* Analog and Digital gains both must be programmed for their minimum values */
		sensor_write_register(APGC_ADDR, 0x00);
        sensor_write_register(APGC_ADDR + 1, 0x00);
		sensor_write_register(DPGC_ADDR, 0x00);
	}
}

static __inline HI_U32 cmos_get_ISO(cmos_gains_ptr_t p_gains)
{
	HI_U32 _again;
	HI_U32 _dgain;

    _again = p_gains->again;
    _dgain = p_gains->dgain;
	p_gains->iso =  (((_again) * (_dgain) * 100) >> (4 + 4));

	return p_gains->iso;
}

/* Note: The unit of return value is 1db.  */
static HI_U8 cmos_get_analog_gain(cmos_gains_ptr_t cmos_gains)
{
    return (cmos_gains->again_db *  3 / 10); 
}

/* Note: The unit of return value is 1db.  */
static HI_U8 cmos_get_digital_gain(cmos_gains_ptr_t cmos_gains)
{
    return  (cmos_gains->dgain_db *  3 / 10); 
}

/*
static HI_U8 cmos_get_digital_fine_gain(cmos_gains_ptr_t cmos_gains)
{
    return cmos_gains->dgain_fine;
}
*/

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
    return isp_special_alg_awb;
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
	.pfn_cmos_get_digital_fine_gain = NULL,
    .pfn_cmos_get_iso = cmos_get_ISO,

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
