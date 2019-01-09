#ifndef __HI_MIPI__
#define __HI_MIPI__

#include "hi_type.h"

typedef unsigned int MIPI_PHY;
typedef unsigned int COMBO_LINK;

#define LVDS_MIN_WIDTH      32
#define LVDS_MIN_HEIGHT     32

#define COMBO_MAX_LINK_NUM  2   /* hi3518ev200 has 1 links, For compatibility of Hi3516A, use 2 */
#define COMBO_MAX_LANE_NUM  8
#define LANE_NUM_PER_LINK   4   /* one link has 4 lanes */


#define MIPI_LANE_NUM       COMBO_MAX_LANE_NUM   /* hi3518ev200 support 1 link mipi only, For compatibility of Hi3516A, use 2*/
#define LVDS_LANE_NUM       COMBO_MAX_LANE_NUM   /* hi3518ev200 has 1 links, so has 4 lanes. For compatibility of Hi3516A, use 8 */


#define WDR_VC_NUM          4
#define SYNC_CODE_NUM       4

/* int mask */
#define COMBO_LINK_INT_DEF          (0x00200000)
#define COMBO_LINK_INT_MASK         (0x300000)
//~(COMBO_LINK_INT_DEF)       /* 0: enable int */

#define MIPI_INT1_INT_DEF           (0x1000fff0)
//#define MIPI_INT1_MASK              ~(MIPI_INT1_INT_DEF)       /* 0: enable int */

#define MIPI_INT2_INT_DEF           (0xf000)
//#define MIPI_INT2_MASK              ~(MIPI_INT2_INT_DEF)       /* 0: enable int */

#define MIPI_INT_MASK               0xffffffff

//#define HI_MIPI_REG_DEBUG
//#define HI_MIPI_DEBUG
#ifdef HI_MIPI_DEBUG

#define HI_MSG(x...) \
do { \
	printk("%s->%d: ", __FUNCTION__, __LINE__); \
	printk(x); \
	printk("\n"); \
} while (0)
#else

#define HI_MSG(args...) do { } while (0)
#endif

#define HI_ERR(x...) \
do { \
	printk(KERN_ALERT "%s(%d): ", __FUNCTION__, __LINE__); \
	printk(KERN_ALERT x); \
	printk(KERN_ALERT "\n"); \
} while (0)


typedef enum
{
    MIPI_VC0_NO_MATCH  = 0x1 << 4,        /*VC0通道的帧起始和帧结束不匹配*/
    MIPI_VC1_NO_MATCH  = 0x1 << 5,        /*VC1通道的帧起始和帧结束不匹配*/
    MIPI_VC2_NO_MATCH  = 0x1 << 6,        /*VC2通道的帧起始和帧结束不匹配*/
    MIPI_VC3_NO_MATCH  = 0x1 << 7,        /*VC3通道的帧起始和帧结束不匹配*/
    
    MIPI_VC0_ORDER_ERR = 0x1 << 8,        /*VC0的帧序出错*/
    MIPI_VC1_ORDER_ERR = 0x1 << 9,        /*VC1的帧序出错*/
    MIPI_VC2_ORDER_ERR = 0x1 << 10,       /*VC2的帧序出错*/
    MIPI_VC3_ORDER_ERR = 0x1 << 11,       /*VC3的帧序出错*/

    MIPI_VC0_FRAME_CRC = 0x1 << 12,        /*最后一帧数据中，VC0通道数据至少有一个CRC错误*/
    MIPI_VC1_FRAME_CRC = 0x1 << 13,        /*最后一帧数据中，VC1通道数据至少有一个CRC错误*/
    MIPI_VC2_FRAME_CRC = 0x1 << 14,        /*最后一帧数据中，VC2通道数据至少有一个CRC错误*/
    MIPI_VC3_FRAME_CRC = 0x1 << 15,        /*最后一帧数据中，VC3通道数据至少有一个CRC错误*/

    MIPI_HEADER_ERR    = 0x1 << 28,        /*Header至少2个错误，ECC无法纠错*/
}MIPI_INT_ERR;

typedef enum
{
    MIPI_VC0_INVALID_DT = 0x1 << 12,        /*VC0通道数据类型不支持*/    
    MIPI_VC1_INVALID_DT = 0x1 << 13,        /*VC1通道数据类型不支持*/
    MIPI_VC2_INVALID_DT = 0x1 << 14,        /*VC2通道数据类型不支持*/
    MIPI_VC3_INVALID_DT = 0x1 << 15,        /*VC3通道数据类型不支持*/
}MIPI_INT2_ERR;

typedef enum
{
    MIPI_ESC_CLK  = 0x1 << 0,               /*MIPI模式时Link 0 clock lane escape序列中断状态。*/
    MIPI_ESC_D0   = 0x1 << 1,               /*data lane 0 escape序列中断状态*/
    MIPI_ESC_D1   = 0x1 << 2,               /*data lane 1 escape序列中断状态*/
    MIPI_ESC_D2   = 0x1 << 3,               /*data lane 2 escape序列中断状态*/
    MIPI_ESC_D3   = 0x1 << 4,               /*data lane 3 escape序列中断状态*/

    MIPI_TIMEOUT_CLK = 0x1 << 8,            /*clock lane FSM timeout 中断状态*/
    MIPI_TIMEOUT_D0  = 0x1 << 9,            /*data lane 0 FSM timeout 中断状态*/
    MIPI_TIMEOUT_D1  = 0x1 << 10,           /*data lane 1 FSM timeout 中断状态*/
    MIPI_TIMEOUT_D2  = 0x1 << 11,           /*data lane 2 FSM timeout 中断状态*/
    MIPI_TIMEOUT_D3  = 0x1 << 12,           /*data lane 3 FSM timeout 中断状态*/

    MIPI_VSYNC_ERR   = 0x1 << 21,           /* LVDS模式，Link 0 各个lane 帧同步信号vsync有效不同步中断状态。*/
}LINK_INT_STAT;

typedef enum
{
    LANE0_SYNC_ERR = 0x1,
    LANE1_SYNC_ERR = 0x2,
    LANE2_SYNC_ERR = 0x4,
    LANE3_SYNC_ERR = 0x8,
    LANE4_SYNC_ERR = 0x10,
    LANE5_SYNC_ERR = 0x20,
    LANE6_SYNC_ERR = 0x40,
    LANE7_SYNC_ERR = 0x80,

    PHY0_LINE_SYNC_ERR  = 0x100,
    PHY0_FRAME_SYNC_ERR = 0x200,
    PHY1_LINE_SYNC_ERR  = 0x400,
    PHY1_FRAME_SYNC_ERR = 0x800,
    LINK_SYNC_ERR       = 0x3F0000,  /* link的行/帧结束信号无法正常检测中断状态，如果有此中断则要重启link。*/
}LVDS_SYNC_INTR_ERR;

typedef enum
{
    OUTPUT_DATA_WIDTH_2BIT=0,
    OUTPUT_DATA_WIDTH_4BIT,
    OUTPUT_DATA_WIDTH_8BIT,
    OUTPUT_DATA_WIDTH_BUTT
}output_data_width;

typedef enum
{
    CLK_UP_EDGE=0,
    CLK_DOWN_EDGE,
    CLK_EDGE_BUTT
}clk_edge;

typedef enum
{
    OUTPUT_NORM_MSB=0,
    OUTPUT_REVERSE_MSB,
    OUTPUT_MSB_BUTT
}output_msb;

typedef enum
{
    INPUT_MODE_MIPI         = 0x0,              /* mipi */
    INPUT_MODE_SUBLVDS      = 0x1,              /* SUB_LVDS */
    INPUT_MODE_LVDS         = 0x2,              /* LVDS */
    INPUT_MODE_HISPI        = 0x3,              /* HISPI */
    INPUT_MODE_CMOS_18V     = 0x4,              /* CMOS 1.8V */
    INPUT_MODE_CMOS_33V     = 0x5,              /* CMOS 3.3V */
    INPUT_MODE_BT1120       = 0x6,              /* CMOS 3.3V */
    INPUT_MODE_BYPASS       = 0x7,              /* MIPI Bypass */
    
    INPUT_MODE_BUTT
}input_mode_t;

typedef enum
{
    WORK_MODE_LVDS          = 0x0,
    WORK_MODE_MIPI          = 0x1,
    WORK_MODE_CMOS_18V      = 0x2,
    WORK_MODE_CMOS_33V      = 0x4,
    WORK_MODE_BT1120        = 0x4,
    WORK_MODE_BUTT
}work_mode_t;

typedef struct
{
    unsigned int width;
    unsigned int height;
}img_size_t;

typedef enum
{
    HI_WDR_MODE_NONE 	= 0x0,
    HI_WDR_MODE_2F   	= 0x1,
    HI_WDR_MODE_3F   	= 0x2,
    HI_WDR_MODE_4F   	= 0x3,
    HI_WDR_MODE_DOL_2F  = 0x4,
    HI_WDR_MODE_DOL_3F  = 0x5,
    HI_WDR_MODE_DOL_4F  = 0x6,
    HI_WDR_MODE_BUTT
}wdr_mode_e;

typedef enum
{
    LVDS_ENDIAN_LITTLE  = 0x0,
    LVDS_ENDIAN_BIG     = 0x1,
    LVDS_ENDIAN_BUTT
}lvds_bit_endian;

typedef enum
{
    LVDS_SYNC_MODE_SOL = 0,         /* sensor SOL, EOL, SOF, EOF */
    LVDS_SYNC_MODE_SAV,             /* SAV, EAV */
    LVDS_SYNC_MODE_BUTT
}lvds_sync_mode_e;

typedef enum
{
    RAW_DATA_8BIT = 1,
    RAW_DATA_10BIT,
    RAW_DATA_12BIT,
    RAW_DATA_14BIT,
    RAW_DATA_BUTT
}raw_data_type_e;

typedef struct
{
    img_size_t          img_size;                   /* oringnal sensor input image size */
    wdr_mode_e          wdr_mode;                   /* WDR mode */
    lvds_sync_mode_e    sync_mode;                  /* sync mode: SOL, SAV */
    raw_data_type_e     raw_data_type;              /* raw data type: 8/10/12/14 bit */
    lvds_bit_endian     data_endian;                /* data endian: little/big */
    lvds_bit_endian     sync_code_endian;           /* sync code endian: little/big */
    short               lane_id[LVDS_LANE_NUM];     /* lane_id: -1 - disable */

    /* each vc has 4 params, sync_code[i]:
       sync_mode is SYNC_MODE_SOL: SOF, EOF, SOL, EOL
       sync_mode is SYNC_MODE_SAV: invalid sav, invalid eav, valid sav, valid eav  */
    unsigned short      sync_code[LVDS_LANE_NUM][WDR_VC_NUM][SYNC_CODE_NUM];
}lvds_dev_attr_t;

typedef struct
{
    raw_data_type_e       raw_data_type;            /* raw data type: 8/10/12/14 bit */
    short                 lane_id[MIPI_LANE_NUM];   /* lane_id: -1 - disable */
}mipi_dev_attr_t;

typedef struct
{
    input_mode_t          input_mode;               /* input mode: MIPI/LVDS/SUBLVDS/HISPI/DC */

    union
    {
        mipi_dev_attr_t     mipi_attr;
        lvds_dev_attr_t     lvds_attr;
    };
}combo_dev_attr_t;


#define HI_MIPI_IOC_MAGIC   'm'

/* init data lane, input mode, data type */
#define HI_MIPI_SET_DEV_ATTR		        _IOW(HI_MIPI_IOC_MAGIC, 0x01, combo_dev_attr_t)

/* output clk edge */
#define HI_MIPI_SET_OUTPUT_CLK_EDGE         _IOW(HI_MIPI_IOC_MAGIC, 0x02, HI_BOOL)

/* output data msb */
#define HI_MIPI_SET_OUTPUT_MSB              _IOW(HI_MIPI_IOC_MAGIC, 0x03, HI_BOOL)

#endif

