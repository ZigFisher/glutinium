#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <assert.h>

#include "hp_common.h"
#include "ps3210k_api.h"
#include "public_api.h"



static int PS3210k_init(void);
static int PS3210k_exit(void);
static void *PS3210k_auto_adjust(void *p);
static void PS3210k_config(void);
static int PS3210k_GetDenoiseLevel(int intTimes, float gainTimes);
static void PS3210k_SetExposure(int level);
static void PS3210k_SetGamma(int level);
static void PS3210k_SetCCM(int level);
static void PS3210k_SetSharpen(int level);
static void PS3210k_SetNRDenoise(int level);
static void PS3210k_SetNR3D(int level);
static void PS3210k_SetUVNR(int level);
static void PS3210k_SetDRC(int level);
static void PS3210k_SetDCI(int level);
static void PS3210k_SetCrosstalk(int level);
static void PS3210k_SetSaturation(int level);
static void PS3210k_SetDefog(int level);


static int PS3210k_ircut_switch(int mode);
static int PS3210k_get_infraredTypeOrYAVG(IRCUT_SWITCH_MODE IRCut_switch_mode);



static pthread_t po1210k_autoAdjust_threadID = -1;

const public_sensor_interface_st PS3210k_sensor =
{
	.sensor_init = &PS3210k_init,
	.sensor_exit = &PS3210k_exit,
	.sensor_ircut_witch = &PS3210k_ircut_switch,
    .sensor_get_infraredTypeOrYAVG = &PS3210k_get_infraredTypeOrYAVG,
};


/*== Gamma start ==*/
const HI_U16 GammaAttr[][GAMMA_NODE_NUM]= 
{
	/* day */
	{0x0  , 0x41 , 0x82 , 0xC5 , 0x109, 0x14C, 0x190, 0x1D4, 0x217, 0x25A, 0x29B, 0x2DB, 0x31A, 0x356, 0x390, 0x3C8, 
	0x3FD, 0x42F, 0x460, 0x48E, 0x4BB, 0x4E6, 0x50F, 0x537, 0x55D, 0x583, 0x5A7, 0x5CA, 0x5EC, 0x60E, 0x62F, 0x64F, 
	0x66F, 0x68E, 0x6AC, 0x6C8, 0x6E4, 0x6FE, 0x717, 0x730, 0x748, 0x75F, 0x775, 0x78C, 0x7A1, 0x7B7, 0x7CC, 0x7E1, 
	0x7F6, 0x80B, 0x81F, 0x833, 0x846, 0x859, 0x86C, 0x87E, 0x88F, 0x8A1, 0x8B2, 0x8C2, 0x8D3, 0x8E3, 0x8F3, 0x903, 
	0x913, 0x923, 0x932, 0x940, 0x94F, 0x95D, 0x96B, 0x979, 0x986, 0x994, 0x9A1, 0x9AE, 0x9BA, 0x9C7, 0x9D4, 0x9E0, 
	0x9ED, 0x9F9, 0xA06, 0xA12, 0xA1E, 0xA2A, 0xA35, 0xA41, 0xA4C, 0xA58, 0xA63, 0xA6E, 0xA79, 0xA84, 0xA90, 0xA9B, 
	0xAA6, 0xAB1, 0xABC, 0xAC8, 0xAD3, 0xADE, 0xAE9, 0xAF4, 0xAFF, 0xB0A, 0xB15, 0xB20, 0xB2B, 0xB35, 0xB40, 0xB4B, 
	0xB55, 0xB5F, 0xB6A, 0xB74, 0xB7E, 0xB89, 0xB93, 0xB9D, 0xBA7, 0xBB1, 0xBBB, 0xBC4, 0xBCE, 0xBD8, 0xBE1, 0xBEB, 
	0xBF4, 0xBFD, 0xC06, 0xC0F, 0xC18, 0xC21, 0xC29, 0xC32, 0xC3A, 0xC43, 0xC4B, 0xC54, 0xC5C, 0xC65, 0xC6E, 0xC76, 
	0xC7F, 0xC88, 0xC91, 0xC9A, 0xCA3, 0xCAC, 0xCB5, 0xCBE, 0xCC7, 0xCD0, 0xCD9, 0xCE2, 0xCEB, 0xCF4, 0xCFD, 0xD06, 
	0xD0F, 0xD18, 0xD21, 0xD2A, 0xD33, 0xD3D, 0xD46, 0xD4F, 0xD58, 0xD61, 0xD6A, 0xD73, 0xD7C, 0xD85, 0xD8E, 0xD96, 
	0xD9F, 0xDA7, 0xDB0, 0xDB8, 0xDC0, 0xDC8, 0xDD0, 0xDD8, 0xDE0, 0xDE8, 0xDF0, 0xDF8, 0xE00, 0xE08, 0xE10, 0xE18, 
	0xE20, 0xE28, 0xE30, 0xE38, 0xE41, 0xE49, 0xE51, 0xE59, 0xE61, 0xE69, 0xE72, 0xE7A, 0xE82, 0xE8A, 0xE92, 0xE9A, 
	0xEA2, 0xEAA, 0xEB2, 0xEBA, 0xEC2, 0xEC9, 0xED1, 0xED9, 0xEE1, 0xEE8, 0xEF0, 0xEF8, 0xEFF, 0xF07, 0xF0F, 0xF16, 
	0xF1E, 0xF26, 0xF2D, 0xF35, 0xF3C, 0xF44, 0xF4C, 0xF53, 0xF5B, 0xF62, 0xF6A, 0xF71, 0xF78, 0xF80, 0xF87, 0xF8E, 
	0xF95, 0xF9C, 0xFA3, 0xFAA, 0xFB0, 0xFB7, 0xFBE, 0xFC4, 0xFCB, 0xFD1, 0xFD8, 0xFDE, 0xFE5, 0xFEB, 0xFF2, 0xFF8, 
	0xFFF,  },
	
	/*night*/
	{0x0  , 0x36 , 0x6A , 0x9E , 0xD1 , 0x103, 0x134, 0x164, 0x193, 0x1C2, 0x1EF, 0x21C, 0x248, 0x274, 0x29E, 0x2C9, 
	0x2F2, 0x31B, 0x343, 0x36A, 0x391, 0x3B7, 0x3DD, 0x402, 0x426, 0x44A, 0x46D, 0x490, 0x4B3, 0x4D4, 0x4F6, 0x517, 
	0x537, 0x557, 0x576, 0x595, 0x5B4, 0x5D2, 0x5F0, 0x60D, 0x62A, 0x647, 0x663, 0x67F, 0x69A, 0x6B5, 0x6D0, 0x6EA, 
	0x704, 0x71E, 0x738, 0x751, 0x769, 0x782, 0x79A, 0x7B2, 0x7C9, 0x7E1, 0x7F8, 0x80E, 0x825, 0x83B, 0x851, 0x866, 
	0x87C, 0x891, 0x8A6, 0x8BB, 0x8CF, 0x8E3, 0x8F7, 0x90B, 0x91F, 0x932, 0x945, 0x958, 0x96B, 0x97D, 0x98F, 0x9A1, 
	0x9B3, 0x9C5, 0x9D7, 0x9E8, 0x9F9, 0xA0A, 0xA1B, 0xA2B, 0xA3C, 0xA4C, 0xA5C, 0xA6C, 0xA7C, 0xA8C, 0xA9B, 0xAAB, 
	0xABA, 0xAC9, 0xAD8, 0xAE6, 0xAF5, 0xB03, 0xB12, 0xB20, 0xB2E, 0xB3C, 0xB4A, 0xB57, 0xB65, 0xB72, 0xB80, 0xB8D, 
	0xB9A, 0xBA7, 0xBB4, 0xBC0, 0xBCD, 0xBD9, 0xBE6, 0xBF2, 0xBFE, 0xC0A, 0xC16, 0xC22, 0xC2E, 0xC39, 0xC45, 0xC50, 
	0xC5C, 0xC67, 0xC72, 0xC7D, 0xC88, 0xC93, 0xC9E, 0xCA8, 0xCB3, 0xCBE, 0xCC8, 0xCD2, 0xCDD, 0xCE7, 0xCF1, 0xCFB, 
	0xD05, 0xD0F, 0xD18, 0xD22, 0xD2C, 0xD35, 0xD3F, 0xD48, 0xD52, 0xD5B, 0xD64, 0xD6D, 0xD76, 0xD7F, 0xD88, 0xD91, 
	0xD9A, 0xDA2, 0xDAB, 0xDB4, 0xDBC, 0xDC5, 0xDCD, 0xDD5, 0xDDE, 0xDE6, 0xDEE, 0xDF6, 0xDFE, 0xE06, 0xE0E, 0xE16, 
	0xE1E, 0xE25, 0xE2D, 0xE35, 0xE3C, 0xE44, 0xE4C, 0xE53, 0xE5A, 0xE62, 0xE69, 0xE70, 0xE77, 0xE7F, 0xE86, 0xE8D, 
	0xE94, 0xE9B, 0xEA2, 0xEA8, 0xEAF, 0xEB6, 0xEBD, 0xEC3, 0xECA, 0xED1, 0xED7, 0xEDE, 0xEE4, 0xEEB, 0xEF1, 0xEF7, 
	0xEFE, 0xF04, 0xF0A, 0xF10, 0xF17, 0xF1D, 0xF23, 0xF29, 0xF2F, 0xF35, 0xF3B, 0xF41, 0xF46, 0xF4C, 0xF52, 0xF58, 
	0xF5D, 0xF63, 0xF69, 0xF6E, 0xF74, 0xF7A, 0xF7F, 0xF85, 0xF8A, 0xF8F, 0xF95, 0xF9A, 0xF9F, 0xFA5, 0xFAA, 0xFAF, 
	0xFB4, 0xFBA, 0xFBF, 0xFC4, 0xFC9, 0xFCE, 0xFD3, 0xFD8, 0xFDD, 0xFE2, 0xFE7, 0xFEC, 0xFF1, 0xFF5, 0xFFA, 0xFFF, 
	0xFFF,  }, 	
};
/*== Gamma end ==*/


/*== ExposureAttr start ==*/
const HI_U16 AEAttr[][2]=
{
	/*Compensation   EVBias*/
	{62,			1024,},	
	{63,			1024,},	
	{64,			1024,},			
};
/*== ExposureAttr end ==*/

/*== CCM start ==*/
const HI_U16 au16CCM[][3][9] =
{  
	{
		{
		 	0x230, 	0x8135, 0x8015,
			0x8075, 0x265, 	0x80a0,
			0x8025, 0x8100, 0x230
		},
		{
			0x260, 	0x8070, 0x8100,
			0x80a2, 0x255, 	0x8070,
			0x8050, 0x81b0, 0x300
		},
		{
			0x280, 	0x8020, 0x8130,
			0x80a5, 0x240,	0x8030,
			0x8075, 0x81b0,	0x320
		}
	},//gain:0-7
	
	{
		{
		 	0x230, 	0x8140, 0x8010,
			0x8065, 0x290, 	0x8030,
			0x8025, 0x8110, 0x240
		},
		{
			0x260, 	0x8070, 0x8100,
			0x80a2, 0x255, 	0x8070,
			0x8050, 0x81b0, 0x300
		},
		{
			0x280, 	0x8020, 0x8130,
			0x80a5, 0x240,	0x8030,
			0x8075, 0x81b0,	0x320
		}
	},//gain:7-20
	
	{
		{

		 	0x1ff, 	0x0, 	0x0,
			0x0, 	0x1ff, 	0x0,
			0x0, 	0x0, 	0x1ff
		},
		{
			0x1ff, 	0x0, 	0x0,
			0x0, 	0x1ff, 	0x0,
			0x0, 	0x0, 	0x1ff
		},
		{
			0x1ff, 	0x0, 	0x0,
			0x0, 	0x1ff, 	0x0,
			0x0, 	0x0, 	0x1ff
		}
	},//gain:20-32
	
};

/*== CCM end ==*/

/*== NR/3DNR start ==*/
const VPSS_NR_PARAM_U NrParam_3Ddenoise[]=
{
	{15, 115,64,  12,  0,0,   30,22,20,  32,40,16,1}, //gain:0-7
	{10, 123,64,  12,  0,0,   30,20,16,  64,80,32,1},//gain:7-20
	{10, 130,64,  11,  0,0,   30,19,12,  64,80,32,1},//gain:20-32  	
};

const HI_U8 au8VarStrength[][16] = 
{
	/*1        2         4        8        16      32       64    128*/
	{160,  160,  180,  180,  180,  180,  180, 180,  150,  150,150,150,150,150,150,150,150},
	
}; 

const HI_U8 au8FixStrength[][16] = 
{
	/*1 	  2    4     8 	 16	 32	64	 128*/
	{0,  0,  0,  0,  0,  0,  0,  0,  110,  110,110,110,110,110,110,110},
	
}; 

const HI_U8 au8LowFreqSlope[][16] =
{
	/*1 	  2 	  4	  8 	 16   32	64   128*/
	{0,  0,  0,  0,  0,  0,	0,  0,	6,  6,	6,  6,	6,  6,	6,  6},
	
};

const HI_U16 au16Threshold[][16] =  
{
	/*1 	    2 	4	  8 	    16    32	64   128*/
	{1500,1500,2800,2800,2800,2800,2800,2800,1375,1375,1375,1375,1375,1375,1375,1375},
};
/*== NR/3DNR end ==*/



/*== Sharpen start ==*/
const HI_U8 au8SharpenD[][16] = 
{
	/*1      2       4      8      16     32     64    128*/
	{150, 145, 139, 133, 110, 100, 100, 60,  60,60,60,60,60,60,60,60},
}; 

const HI_U8 au8SharpenUd[][16] = 
{
	/*1      2       4      8      16     32     64    128*/
	{115, 112, 98,  95,  90,  80,  80,  80,  70,70,70,70,70,70,70,70},
};

HI_U8 au8OverShoot[][16] = 
{
	/*1      2       4      8      16     32     64    128*/
	{70,  68,  64,  50,  48,  32,  36,  40,  40,40,40,40,40,40,40,40},
};

const HI_U8 au8UnderShoot[][16] = 
{
	/*1      2       4      8      16     32     64    128*/
	{107, 100, 91,	64,  55,  40,  47,	54,  80,80,80,80,80,80,80,80},
};

const HI_U8 au8TextureNoiseThd[][16] =
{
	/*1 	  2 	 4	8    16    32	64   128*/
	{0,  0,  0,	0,	0,	1,	1,	1,	1,1,1,1,1,1,1,1},
};

const HI_U8 au8EdgeNoiseThd[][16] =
{
	/*1 	  2 	 4	8     16  32    64   128*/
	{0,  0,  0,	0,	0,	1,	1,	1,	1,1,1,1,1,1,1,1},
};

const HI_BOOL abEnLowLumaShoot[][16] = 
{
	/*1 	  2 	 4	8    16   32   64    128*/
	{0,  0,  0,	0,	0,	1,	1,	1,	1,1,1,1,1,1,1,1},
};
/*== Sharpen end ==*/

/*== UVNR start ==*/
const HI_U8 au8ColorCast[][16] = 
{
	/*1 	  2 	 4	8    16   32   64    128*/
	{0,  0,  2,	3,	3,	3,	3,	3,	1,1,1,1,1,1,1,1},
};	

const HI_U8 au8UvnrThreshold[][16] = 
{
	/*1 	  2 	 4	 8     16    32   64    128*/
	{20, 20, 20, 20, 20, 20, 20,  20,	1,1,1,1,1,1,1,1},
};	

const HI_U8 au8UvnrStrength[][16] = 
{
	/*1 	  2 	  4	  8    16    32    64    128*/
	{34, 34, 34, 34, 34, 34, 34,  34,	1,1,1,1,1,1,1,1},
};	
/*== UVNR end ==*/

/*== DRC start==*/
const ISP_DRC_ATTR_S stDRC[]=
	{   /*
		HI_BOOL bEnable;

		HI_U8  u8SpatialVar;             
		HI_U8  u8RangeVar;               
		
		HI_U8  u8Asymmetry;              
		HI_U8  u8SecondPole;             
		HI_U8  u8Stretch;                
		HI_U8  u8LocalMixingThres;       

		HI_U16 u16DarkGainLmtY;         
		HI_U16 u16DarkGainLmtC;          
		HI_U16 u16BrightGainLmt;         

	    	ISP_OP_TYPE_E enOpType;
	    	ISP_DRC_MANUAL_ATTR_S stManual;
	    	ISP_DRC_AUTO_ATTR_S   stAuto;
	    	*/
	{1,  10,0,  2,192,60,2,  0,48,0,  OP_TYPE_MANUAL,  {40,0,32},  {128,54,32},},
	{1,  10,0,  2,192,60,2,  0,48,0,  OP_TYPE_MANUAL,  {20,0,32},  {128,54,32},},
	{0,  10,0,  2,192,60,2,  0,48,0,  OP_TYPE_MANUAL,  {20,0,32},  {128,54,32},},
};
/*== DRC end==*/

/*== DCI start==*/
const VI_DCI_PARAM_S stDciParam[]=
{
	{1, 40,45,0 },
	{1, 50,55,40},
	{1, 63,63,63},
};
/*== DCI end==*/

/*== Crosstalk Removal start==*/
const ISP_CR_ATTR_S stCRAttr[]=
{
	{1,{1024,1024,1024,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048},8192,7,7,8192},
};
/*== Crosstalk Removal end==*/

/*== Saturation start ==*/
const HI_U8 au8Saturation[][16] = 
{
	/*1 	     2 	 4	  8    16      32    64    128*/
	{135, 128, 120, 110, 100, 78,  70,  60,56,56,56,56,56,56,56,56},
	{135, 135, 120, 120, 113, 90,  90,  60,56,56,56,56,56,56,56,56},
	{200, 200, 200, 200, 200, 180, 180, 180,56,56,56,56,56,56,56,56},
};	
/*== Saturation end ==*/

/*== Defog start ==*/
ISP_DEFOG_ATTR_S stDefogAttr[]=
{
	{1, 16,15,OP_TYPE_MANUAL, {100},{128}},
	{0, 16,15,OP_TYPE_MANUAL, {100},{128}},
};
/*== Defog end ==*/

int PS3210k_init(void)
{
	pthread_create(&po1210k_autoAdjust_threadID, NULL, PS3210k_auto_adjust, NULL);
	return 0;
}
int PS3210k_exit(void)
{
	//pthread_cancel(po1210k_autoAdjust_threadID);
	//pthread_jion();
	return 0;
}

void PS3210k_config(void)
{
	ISP_DEV IspDev = 0;
	/*== ExposureAttr ==*/
	ISP_EXPOSURE_ATTR_S stExpAttr;
	HI_MPI_ISP_GetExposureAttr(IspDev, &stExpAttr);
	stExpAttr.stAuto.stExpTimeRange.u32Max = 70000;
	stExpAttr.stAuto.stAGainRange.u32Max = 16384;	
	stExpAttr.stAuto.stDGainRange.u32Max = 1024;
	stExpAttr.stAuto.stISPDGainRange.u32Max = 2048;
	stExpAttr.stAuto.enAEMode = AE_MODE_SLOW_SHUTTER;
	HI_MPI_ISP_SetExposureAttr(IspDev,&stExpAttr); 
	
	/*== Demosaic ==*/
	ISP_DEMOSAIC_ATTR_S stDemosaicAttr;
	//HI_MPI_ISP_GetDemosaicAttr(IspDev, &stDemosaicAttr);
	//HI_MPI_ISP_SetDemosaicAttr(IspDev, &stDemosaicAttr);

	PS3210k_SetExposure(0);
	PS3210k_SetGamma(0);
	PS3210k_SetCCM(0);
	PS3210k_SetSaturation(0);
	PS3210k_SetSharpen(0);
	PS3210k_SetNRDenoise(0);
	PS3210k_SetNR3D(0);
	PS3210k_SetUVNR(0);
	PS3210k_SetDRC(0);
	PS3210k_SetDCI(0);
	PS3210k_SetCrosstalk(0);
	
}

void *PS3210k_auto_adjust(void *p)
{	
	ISP_EXP_INFO_S stExpInfo;
	ISP_DEV IspDev = 0;
	float GainTimes;
	int IntTimeLines;
	int level = 0;
	int prelevel = -1;

	usleep(1000000); //wait sensor init ,vi ,vpss, venc start
	
	PS3210k_config();

	/**** auto denoise ***/
	while(1)
	{
		HI_MPI_ISP_QueryExposureInfo(IspDev, &stExpInfo);
		GainTimes = (stExpInfo.u32AGain * stExpInfo.u32ISPDGain) / (1024.0 * 1024.0);
		IntTimeLines = stExpInfo.u32ExpTime / 29.63;  //25fps, the time of 1 line is 29.63us
		
		level = PS3210k_GetDenoiseLevel(IntTimeLines,GainTimes);
		
		if(prelevel != level)
		{	
			printf("level = %d\n",level);
			HI_MPI_ISP_SetFMWState(IspDev,ISP_FMW_STATE_FREEZE);
			PS3210k_SetExposure(level);
			PS3210k_SetNR3D(level);
			PS3210k_SetGamma(level);
			PS3210k_SetCCM(level);
			PS3210k_SetDCI(level);
			PS3210k_SetDRC(level);
			PS3210k_SetSaturation(level);
			HI_MPI_ISP_SetFMWState(IspDev,ISP_FMW_STATE_RUN);
			
		}
		prelevel = level;

		/*only use for Defog*/
		if(IntTimeLines < 100)
		{
			PS3210k_SetDefog(0);
		}
		else
		{
			PS3210k_SetDefog(1);
		}
		/*only use for Defog*/
		
		usleep(30000);
	}
	
	return NULL;	
}
int PS3210k_GetDenoiseLevel(int intTimes, float gainTimes)
{
	int level_t;
	if(gainTimes < 7.0)
	{
		level_t = 0;
	}
	else if(gainTimes < 20.0)
	{
		level_t = 1;
	}
	else
	{
		level_t = 2;
	}
	
	return level_t;
}


/*== Gamma ==*/
void PS3210k_SetGamma(int level)
{
	ISP_DEV IspDev = 0;
	ISP_GAMMA_ATTR_S stGammaAttr;
	level = level / 2;
	HI_MPI_ISP_GetGammaAttr(IspDev, &stGammaAttr);
	stGammaAttr.enCurveType = ISP_GAMMA_CURVE_USER_DEFINE;
	memcpy(stGammaAttr.u16Table,&GammaAttr[level],sizeof(stGammaAttr.u16Table));
	HI_MPI_ISP_SetGammaAttr(IspDev, &stGammaAttr);
}

/*== CCM ==*/
void PS3210k_SetCCM(int level)
{
	ISP_DEV IspDev = 0;
	ISP_COLORMATRIX_ATTR_S stCCMAttr;
	HI_MPI_ISP_GetCCMAttr(IspDev, &stCCMAttr);
	memcpy(stCCMAttr.stAuto.au16HighCCM,&au16CCM[level][0],sizeof(stCCMAttr.stAuto.au16HighCCM));
	memcpy(stCCMAttr.stAuto.au16MidCCM,&au16CCM[level][1],sizeof(stCCMAttr.stAuto.au16MidCCM));
	memcpy(stCCMAttr.stAuto.au16LowCCM,&au16CCM[level][2],sizeof(stCCMAttr.stAuto.au16LowCCM));
	HI_MPI_ISP_SetCCMAttr(IspDev, &stCCMAttr);
}


/*== ExposureAttr ==*/
void PS3210k_SetExposure(int level)
{
	ISP_DEV IspDev = 0;
	ISP_EXPOSURE_ATTR_S stExpAttr;
	HI_MPI_ISP_GetExposureAttr(IspDev, &stExpAttr);
	stExpAttr.stAuto.u8Compensation = AEAttr[level][0];
	stExpAttr.stAuto.u16EVBias = AEAttr[level][1];
	HI_MPI_ISP_SetExposureAttr(IspDev,&stExpAttr); 
}

/*== Sharpen ==*/
void PS3210k_SetSharpen(int level)
{
	ISP_DEV IspDev = 0;
	ISP_SHARPEN_ATTR_S stSharpenAttr;
	HI_MPI_ISP_GetSharpenAttr(IspDev, &stSharpenAttr);
	memcpy(stSharpenAttr.stAuto.au8SharpenD, &au8SharpenD[level], sizeof(stSharpenAttr.stAuto.au8SharpenD));
	memcpy(stSharpenAttr.stAuto.au8SharpenUd, &au8SharpenUd[level], sizeof(stSharpenAttr.stAuto.au8SharpenUd));
	memcpy(stSharpenAttr.stAuto.au8OverShoot, &au8OverShoot[level], sizeof(stSharpenAttr.stAuto.au8OverShoot));
	memcpy(stSharpenAttr.stAuto.au8UnderShoot, &au8UnderShoot[level], sizeof(stSharpenAttr.stAuto.au8UnderShoot));
	memcpy(stSharpenAttr.stAuto.au8TextureNoiseThd, &au8TextureNoiseThd[level], sizeof(stSharpenAttr.stAuto.au8TextureNoiseThd));
	memcpy(stSharpenAttr.stAuto.au8EdgeNoiseThd, &au8EdgeNoiseThd[level], sizeof(stSharpenAttr.stAuto.au8EdgeNoiseThd));
	memcpy(stSharpenAttr.stAuto.abEnLowLumaShoot, &abEnLowLumaShoot[level], sizeof(stSharpenAttr.stAuto.abEnLowLumaShoot));
	HI_MPI_ISP_SetSharpenAttr(IspDev, &stSharpenAttr);
}

/*== NR/3DNR==*/
void PS3210k_SetNRDenoise(int level)
{
	ISP_DEV IspDev = 0;
	ISP_NR_ATTR_S stNRAttr;
	HI_MPI_ISP_GetNRAttr(IspDev,&stNRAttr);
	memcpy(stNRAttr.stAuto.au8VarStrength, &au8VarStrength[level], sizeof(stNRAttr.stAuto.au8VarStrength));
	memcpy(stNRAttr.stAuto.au8FixStrength, &au8FixStrength[level], sizeof(stNRAttr.stAuto.au8FixStrength));
	memcpy(stNRAttr.stAuto.au8LowFreqSlope, &au8LowFreqSlope[level], sizeof(stNRAttr.stAuto.au8LowFreqSlope));
	memcpy(stNRAttr.stAuto.au16Threshold, &au16Threshold[level], sizeof(stNRAttr.stAuto.au16Threshold));
	HI_MPI_ISP_SetNRAttr(IspDev,&stNRAttr);
}

void PS3210k_SetNR3D(int level)
{
	VPSS_GRP VpssGrp =0;
	HI_MPI_VPSS_SetNRParam(VpssGrp, &NrParam_3Ddenoise[level]);	
}

/*== UVNR ==*/
void PS3210k_SetUVNR(int level)
{
	ISP_DEV IspDev = 0;
	ISP_UVNR_ATTR_S stUvnrAttr;
	HI_MPI_ISP_GetUVNRAttr(IspDev,&stUvnrAttr);
	memcpy(stUvnrAttr.stAuto.au8ColorCast, &au8ColorCast[level], sizeof(stUvnrAttr.stAuto.au8ColorCast));
	memcpy(stUvnrAttr.stAuto.au8UvnrThreshold, &au8UvnrThreshold[level], sizeof(stUvnrAttr.stAuto.au8UvnrThreshold));
	memcpy(stUvnrAttr.stAuto.au8UvnrStrength, &au8UvnrStrength[level], sizeof(stUvnrAttr.stAuto.au8UvnrStrength));
	HI_MPI_ISP_SetUVNRAttr(IspDev,&stUvnrAttr);
}

/*== DRC ==*/
void PS3210k_SetDRC(int level)
{
	ISP_DEV IspDev = 0;
	HI_MPI_ISP_SetDRCAttr(IspDev,&stDRC[level]);
}

/*== DCI ==*/
void PS3210k_SetDCI(int level)
{
	VI_DEV ViDev = 0;
	HI_MPI_VI_SetDCIParam(ViDev, &stDciParam[level]);
}

/*== Crosstalk Removal ==*/
void PS3210k_SetCrosstalk(int level)
{
	ISP_DEV IspDev = 0;
	HI_MPI_ISP_SetCrosstalkAttr(IspDev,&stCRAttr[level]);
}

/*== Saturation ==*/
void PS3210k_SetSaturation(int level)
{
	ISP_DEV IspDev = 0;
	ISP_SATURATION_ATTR_S stSatAttr;
	HI_MPI_ISP_GetSaturationAttr(IspDev, &stSatAttr);
	memcpy(stSatAttr.stAuto.au8Sat, &au8Saturation[level], sizeof(stSatAttr.stAuto.au8Sat));
	HI_MPI_ISP_SetSaturationAttr(IspDev, &stSatAttr);
}

/*== Defog ==*/
void PS3210k_SetDefog(int level)
{
	ISP_DEV IspDev = 0;
	HI_MPI_ISP_SetDeFogAttr(IspDev, &stDefogAttr[level]);
}

/* IR CUT*/
int PS3210k_ircut_switch(int mode)
{
	int var;
	SysRegWrite(0x200F0100, 0x1);//GPIO8_0
	SysRegWrite(0x200F0104, 0x1);//GPIO8_1
	SysRegRead(0x201C0400, &var);
	var = var | 0x03;
	SysRegWrite(0x201C0400, var);
	if(IRCUT_NIGHT == mode)
	{
		SysRegWrite(0x201C000C ,0x01);
	}
	else
	{
		SysRegWrite(0x201C000C ,0x02);
	}
	usleep(120000);
	SysRegWrite(0x201C000C,0x00);
	return 0;
}
int PS3210k_get_infraredTypeOrYAVG(IRCUT_SWITCH_MODE IRCut_switch_mode)
{
	unsigned int data = 0;
	if( IRCUT_SYN_INFRARED == IRCut_switch_mode)
	{
		SysRegWrite(0x200F00FC,0x01);//gpio7_7
		SysRegRead(0x201B0400, &data);
		data &= (~(1 << 7));
		SysRegWrite(0x201B0400, data);
		
		data = 0;
		SysRegRead(0x201B0200,&data);
		data = ((0 != data) ? 0 : 1);
	}
	else
	{
		data = 0xFFFF;
	}
	return data;
}

