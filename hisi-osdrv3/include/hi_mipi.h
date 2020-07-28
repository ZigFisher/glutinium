#ifndef __HI_MIPI__
#define __HI_MIPI__

#include "hi_type.h"

typedef unsigned int COMBO_LINK;
typedef unsigned int COMBO_DEV;

#define LVDS_MIN_WIDTH      32
#define LVDS_MIN_HEIGHT     32

#define COMBO_MAX_DEV_NUM   1   /* hi3516CV300 support 1 sensors */
#define COMBO_MAX_LINK_NUM  1   /* hi3516CV300 has 1 links */
#define COMBO_MAX_LANE_NUM  4
#define LANE_NUM_PER_LINK   4   /* one link has 4 lanes */

#define MIPI_LANE_NUM       (LANE_NUM_PER_LINK * 1)   /* hi3516CV300 mipi support max 1 links */
#define LVDS_LANE_NUM       COMBO_MAX_LANE_NUM        /* hi3516CV300 lvds suppor max 1 links, so has 4 lanes */


#define WDR_VC_NUM          4
#define SYNC_CODE_NUM       4


//#define HI_MIPI_REG_DEBUG
//#define HI_MIPI_DEBUG
#ifdef HI_MIPI_DEBUG

#define HI_MSG(x...) \
    do { \
       osal_printk("%s->%d: ", __FUNCTION__, __LINE__); \
       osal_printk(x); \
       osal_printk("\n"); \
    } while (0)
#else

#define HI_MSG(args...) do { } while (0)
#endif

#define HI_ERR(x...) \
    do { \
        osal_printk("%s(%d): ", __FUNCTION__, __LINE__); \
        osal_printk(x); \
        osal_printk("\n"); \
    } while (0)


typedef enum
{
    CLK_UP_EDGE = 0,
    CLK_DOWN_EDGE,
    CLK_EDGE_BUTT
} clk_edge;

typedef enum
{
    OUTPUT_NORM_MSB = 0,
    OUTPUT_REVERSE_MSB,
    OUTPUT_MSB_BUTT
} output_msb;

typedef enum
{
    INPUT_MODE_MIPI         = 0x0,              /* mipi */
    INPUT_MODE_SUBLVDS      = 0x1,              /* SUB_LVDS */
    INPUT_MODE_LVDS         = 0x2,              /* LVDS */
    INPUT_MODE_HISPI        = 0x3,              /* HISPI */
    INPUT_MODE_CMOS         = 0x4,              /* CMOS */
    INPUT_MODE_BT1120       = 0x5,              /* CMOS */

    INPUT_MODE_BUTT
} input_mode_t;

typedef enum
{
    WORK_MODE_LVDS          = 0x0,
    WORK_MODE_MIPI          = 0x1,
    WORK_MODE_CMOS          = 0x2,
    WORK_MODE_BT1120        = 0x3,
    WORK_MODE_BUTT
} work_mode_t;

typedef struct
{
    unsigned int width;
    unsigned int height;
} img_size_t;

/* LVDS WDR MODE defines */
typedef enum
{
    HI_WDR_MODE_NONE    = 0x0,
    HI_WDR_MODE_2F      = 0x1,
    HI_WDR_MODE_3F      = 0x2,
    HI_WDR_MODE_4F      = 0x3,
    HI_WDR_MODE_DOL_2F  = 0x4,
    HI_WDR_MODE_DOL_3F  = 0x5,
    HI_WDR_MODE_DOL_4F  = 0x6,
    HI_WDR_MODE_BUTT
} wdr_mode_e;

/* MIPI D_PHY WDR MODE defines */
typedef enum
{
    HI_MIPI_WDR_MODE_NONE = 0x0,
    HI_MIPI_WDR_MODE_VC   = 0x1,    /* Virtual Channel */
    HI_MIPI_WDR_MODE_DT   = 0x2,    /* Data Type */
    HI_MIPI_WDR_MODE_DOL  = 0x3,    /* DOL Mode */
    HI_MIPI_WDR_MODE_BUTT
} mipi_wdr_mode_e;

typedef enum
{
    LVDS_ENDIAN_LITTLE  = 0x0,
    LVDS_ENDIAN_BIG     = 0x1,
    LVDS_ENDIAN_BUTT
} lvds_bit_endian;

typedef enum
{
    LVDS_SYNC_MODE_SOF = 0,         /* sensor SOL, EOL, SOF, EOF */
    LVDS_SYNC_MODE_SAV,             /* SAV, EAV */
    LVDS_SYNC_MODE_BUTT
} lvds_sync_mode_e;

typedef enum
{
    RAW_DATA_8BIT = 0,
    RAW_DATA_10BIT,
    RAW_DATA_12BIT,
    RAW_DATA_14BIT,
    RAW_DATA_16BIT,
    RAW_DATA_BUTT
} raw_data_type_e;

typedef enum
{
    LVDS_VSYNC_NORMAL   = 0x00,
    LVDS_VSYNC_SHARE    = 0x01,
    LVDS_VSYNC_HCONNECT = 0x02,
    LVDS_VSYNC_BUTT
} lvds_vsync_type_e;

typedef struct
{
    lvds_vsync_type_e sync_type;

    //hconnect vsync blanking len, valid when the sync_type is LVDS_VSYNC_HCONNECT
    unsigned short hblank1;
    unsigned short hblank2;
} lvds_vsync_type_t;

typedef enum
{
    LVDS_FID_NONE    = 0x00,
    LVDS_FID_IN_SAV  = 0x01,    /* frame identification id in SAV 4th */
    LVDS_FID_IN_DATA = 0x02,    /* frame identification id in first data */
    LVDS_FID_BUTT
} lvds_fid_type_e;

typedef struct
{
    lvds_fid_type_e fid;

    /* Sony DOL has the Frame Information Line, in DOL H-Connection mode,
       should configure this flag as false to disable output the Frame Information Line */
    HI_BOOL output_fil;
} lvds_fid_type_t;

typedef struct
{
    img_size_t          img_size;                   /* oringnal sensor input image size */
    raw_data_type_e     raw_data_type;              /* raw data type: 8/10/12/14 bit */
    wdr_mode_e          wdr_mode;                   /* WDR mode */

    lvds_sync_mode_e    sync_mode;                  /* sync mode: SOF, SAV */
    lvds_vsync_type_t   vsync_type;                 /* normal, share, hconnect */
    lvds_fid_type_t     fid_type;                   /* frame identification code */

    lvds_bit_endian     data_endian;                /* data endian: little/big */
    lvds_bit_endian     sync_code_endian;           /* sync code endian: little/big */
    short               lane_id[LVDS_LANE_NUM];     /* lane_id: -1 - disable */

    /* each vc has 4 params, sync_code[i]:
       sync_mode is SYNC_MODE_SOF: SOL, EOL, SOF, EOF
       sync_mode is SYNC_MODE_SAV: valid sav, valid eav, invalid sav, invalid eav */
    unsigned short      sync_code[LVDS_LANE_NUM][WDR_VC_NUM][SYNC_CODE_NUM];
} lvds_dev_attr_t;

typedef struct
{
    raw_data_type_e       raw_data_type;            /* raw data type: 8/10/12/14 bit */
    mipi_wdr_mode_e       wdr_mode;                 /* MIPI WDR mode */
    short                 lane_id[MIPI_LANE_NUM];   /* lane_id: -1 - disable */

    union
    {
        short data_type[WDR_VC_NUM];                /* used by the HI_MIPI_WDR_MODE_DT */
    };
} mipi_dev_attr_t;

typedef struct
{
    COMBO_DEV               devno;                  /* device number, select sensor0 and sensor 1 */
    input_mode_t            input_mode;             /* input mode: MIPI/LVDS/SUBLVDS/HISPI/DC */

    union
    {
        mipi_dev_attr_t     mipi_attr;
        lvds_dev_attr_t     lvds_attr;
    };
} combo_dev_attr_t;

/* phy common mode voltage mode, greater than 900mv or less than 900mv */
typedef enum
{
    PHY_CMV_GE900MV    = 0x00,
    PHY_CMV_LT900MV    = 0x01,
    PHY_CMV_BUTT
} phy_cmv_e;

typedef struct
{
    COMBO_DEV   devno;
    phy_cmv_e   cmv_mode;
} phy_cmv_t;

typedef struct
{
    int x;
    int y;
    unsigned int width;
    unsigned int height;
} img_rect_t;

#define HI_MIPI_IOC_MAGIC   'm'

/* init data lane, input mode, data type */
#define HI_MIPI_SET_DEV_ATTR                _IOW(HI_MIPI_IOC_MAGIC, 0x01, combo_dev_attr_t)

/* output clk edge */
#define HI_MIPI_SET_OUTPUT_CLK_EDGE         _IOW(HI_MIPI_IOC_MAGIC, 0x02, clk_edge)

/* output data msb */
#define HI_MIPI_SET_OUTPUT_MSB              _IOW(HI_MIPI_IOC_MAGIC, 0x03, output_msb)

/* set phy common mode voltage mode */
#define HI_MIPI_SET_PHY_CMVMODE             _IOW(HI_MIPI_IOC_MAGIC, 0x04, phy_cmv_t)

/* reset sensor */
#define HI_MIPI_RESET_SENSOR                _IOW(HI_MIPI_IOC_MAGIC, 0x05, COMBO_DEV)

/* unreset sensor */
#define HI_MIPI_UNRESET_SENSOR              _IOW(HI_MIPI_IOC_MAGIC, 0x06, COMBO_DEV)

/* reset mipi */
#define HI_MIPI_RESET_MIPI                  _IOW(HI_MIPI_IOC_MAGIC, 0x07, COMBO_DEV)

/* unreset mipi */
#define HI_MIPI_UNRESET_MIPI                _IOW(HI_MIPI_IOC_MAGIC, 0x08, COMBO_DEV)

/* set mipi crop */
#define HI_MIPI_SET_CROP                    _IOW(HI_MIPI_IOC_MAGIC, 0x09, img_rect_t)

#endif
