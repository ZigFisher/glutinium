/***************************************************************************#
# simplified_jpeg_encoder: library to encode a jpeg frame                   #
# from various input palette.																						    #
# simplified_jpeg_encoder works for embedded device without libjpeg         #
#.                                                                          #
#     Copyright (C) 2010  Vladimir S. Fonov                                 #
# Based on jpegenc code from Michel Xhaard   Copyright (C) 2005             #
#                                                                           
# This program is free software; you can redistribute it and/or modify      #
# it under the terms of the GNU General Public License as published by      #
# the Free Software Foundation; either version 2 of the License, or         #
# (at your option) any later version.                                       #
#                                                                           #
# This program is distributed in the hope that it will be useful,           #
# but WITHOUT ANY WARRANTY; without even the implied warranty of            #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             #
# GNU General Public License for more details.                              #
#                                                                           #
# You should have received a copy of the GNU General Public License         #
# along with this program; if not, write to the Free Software               #
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA #
#  CREDIT:                    																							#
# Original code from Nitin Gupta India (?)              										#
#                                                                           #
#***************************************************************************/

#include "simplified_jpeg_encoder.h"


#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <string.h>

#pragma pack(push,1)
typedef struct S_JPEG_RGB16 {
  unsigned short blue:5;
  unsigned short green:6;
  unsigned short red:5;
} S_JPEG_RGB16;
typedef struct S_JPEG_RGB24 {
  unsigned char blue;
  unsigned char green;
  unsigned char red;
} S_JPEG_RGB24;
typedef struct S_JPEG_RGB32 {
  unsigned char blue;
  unsigned char green;
  unsigned char red;
  unsigned char alpha;
} S_JPEG_RGB32;
#pragma pack(pop)

//Quantize interface
INLINE void initialize_quantization_tables(S_JPEG_ENCODER_STRUCTURE * jpeg,uint32_t);
INLINE void quantization(S_JPEG_ENCODER_STRUCTURE * jpeg,S_INT *data, const S_UINT * quant);
INLINE S_UINT Q15_Division_Integer(uint32_t numer, uint32_t denom);

//markers
uint8_t * write_markers(S_JPEG_ENCODER_STRUCTURE *enc,
												uint8_t * output_ptr,
                        uint32_t image_format,
                        uint32_t image_width,
                        uint32_t image_height);

                        
                        
typedef enum COMPONENT_tag { COMPONENT_Y=1,COMPONENT_CB=2,COMPONENT_CR=3} COMPONENT;

// huffman                        
S_PIXEL * huffman(S_JPEG_ENCODER_STRUCTURE *, COMPONENT, S_PIXEL *);
S_PIXEL * close_bitstream(S_JPEG_ENCODER_STRUCTURE *,S_PIXEL *);
                        
static void read_YCbCr400(S_JPEG_ENCODER_STRUCTURE  * enc, uint8_t * input_ptr_,S_UINT row,S_UINT col);
static void read_YCbCr420(S_JPEG_ENCODER_STRUCTURE  * enc, uint8_t * input_ptr_,S_UINT row,S_UINT col);
static void read_YCbCr422(S_JPEG_ENCODER_STRUCTURE  * enc, uint8_t * input_ptr_,S_UINT row,S_UINT col);
static void read_YCbCr444(S_JPEG_ENCODER_STRUCTURE  * enc, uint8_t * input_ptr_,S_UINT row,S_UINT col);
static void read_YCbCr420p(S_JPEG_ENCODER_STRUCTURE  * enc, uint8_t * input_ptr_,S_UINT row,S_UINT col);
static void read_YCbCr422p(S_JPEG_ENCODER_STRUCTURE  * enc, uint8_t * input_ptr_,S_UINT row,S_UINT col);



                      
static void DCT(S_INT * data);

static void initialization(S_JPEG_ENCODER_STRUCTURE * jpeg,
                           uint32_t image_format, 
                           uint32_t image_width, uint32_t image_height);
                           
static S_PIXEL *encodeMCU(S_JPEG_ENCODER_STRUCTURE * enc,
                          uint32_t image_format, S_PIXEL * output_ptr);


S_UINT luminance_dc_code_table[] = {
  0x0000, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x000E, 0x001E, 0x003E,
  0x007E, 0x00FE, 0x01FE
};
S_UINT luminance_dc_size_table[] = {
  0x0002, 0x0003, 0x0003, 0x0003, 0x0003, 0x0003, 0x0004, 0x0005, 0x0006,
  0x0007, 0x0008, 0x0009
};
S_UINT chrominance_dc_code_table[] = {
  0x0000, 0x0001, 0x0002, 0x0006, 0x000E, 0x001E, 0x003E, 0x007E, 0x00FE,
  0x01FE, 0x03FE, 0x07FE
};
S_UINT chrominance_dc_size_table[] = {
  0x0002, 0x0002, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008,
  0x0009, 0x000A, 0x000B
};

S_UINT luminance_ac_code_table[] = {
  0x000A, 0x0000, 0x0001, 0x0004, 0x000B, 0x001A, 0x0078, 0x00F8, 0x03F6,
  0xFF82, 0xFF83, 0x000C, 0x001B, 0x0079, 0x01F6, 0x07F6, 0xFF84, 0xFF85,
  0xFF86, 0xFF87, 0xFF88, 0x001C, 0x00F9, 0x03F7, 0x0FF4, 0xFF89, 0xFF8A,
  0xFF8b, 0xFF8C, 0xFF8D, 0xFF8E, 0x003A, 0x01F7, 0x0FF5, 0xFF8F, 0xFF90,
  0xFF91, 0xFF92, 0xFF93, 0xFF94, 0xFF95, 0x003B, 0x03F8, 0xFF96, 0xFF97,
  0xFF98, 0xFF99, 0xFF9A, 0xFF9B, 0xFF9C, 0xFF9D, 0x007A, 0x07F7, 0xFF9E,
  0xFF9F, 0xFFA0, 0xFFA1, 0xFFA2, 0xFFA3, 0xFFA4, 0xFFA5, 0x007B, 0x0FF6,
  0xFFA6, 0xFFA7, 0xFFA8, 0xFFA9, 0xFFAA, 0xFFAB, 0xFFAC, 0xFFAD, 0x00FA,
  0x0FF7, 0xFFAE, 0xFFAF, 0xFFB0, 0xFFB1, 0xFFB2, 0xFFB3, 0xFFB4, 0xFFB5,
  0x01F8, 0x7FC0, 0xFFB6, 0xFFB7, 0xFFB8, 0xFFB9, 0xFFBA, 0xFFBB, 0xFFBC,
  0xFFBD, 0x01F9, 0xFFBE, 0xFFBF, 0xFFC0, 0xFFC1, 0xFFC2, 0xFFC3, 0xFFC4,
  0xFFC5, 0xFFC6, 0x01FA, 0xFFC7, 0xFFC8, 0xFFC9, 0xFFCA, 0xFFCB, 0xFFCC,
  0xFFCD, 0xFFCE, 0xFFCF, 0x03F9, 0xFFD0, 0xFFD1, 0xFFD2, 0xFFD3, 0xFFD4,
  0xFFD5, 0xFFD6, 0xFFD7, 0xFFD8, 0x03FA, 0xFFD9, 0xFFDA, 0xFFDB, 0xFFDC,
  0xFFDD, 0xFFDE, 0xFFDF, 0xFFE0, 0xFFE1, 0x07F8, 0xFFE2, 0xFFE3, 0xFFE4,
  0xFFE5, 0xFFE6, 0xFFE7, 0xFFE8, 0xFFE9, 0xFFEA, 0xFFEB, 0xFFEC, 0xFFED,
  0xFFEE, 0xFFEF, 0xFFF0, 0xFFF1, 0xFFF2, 0xFFF3, 0xFFF4, 0xFFF5, 0xFFF6,
  0xFFF7, 0xFFF8, 0xFFF9, 0xFFFA, 0xFFFB, 0xFFFC, 0xFFFD, 0xFFFE,
  0x07F9
};
S_UINT luminance_ac_size_table[] = {
  0x0004, 0x0002, 0x0002, 0x0003, 0x0004, 0x0005, 0x0007, 0x0008, 0x000A,
  0x0010, 0x0010, 0x0004, 0x0005, 0x0007, 0x0009, 0x000B, 0x0010, 0x0010,
  0x0010, 0x0010, 0x0010, 0x0005, 0x0008, 0x000A, 0x000C, 0x0010, 0x0010,
  0x0010, 0x0010, 0x0010, 0x0010, 0x0006, 0x0009, 0x000C, 0x0010, 0x0010,
  0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0006, 0x000A, 0x0010, 0x0010,
  0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0007, 0x000B, 0x0010,
  0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0007, 0x000C,
  0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0008,
  0x000C, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
  0x0009, 0x000F, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
  0x0010, 0x0009, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
  0x0010, 0x0010, 0x0009, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
  0x0010, 0x0010, 0x0010, 0x000A, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
  0x0010, 0x0010, 0x0010, 0x0010, 0x000A, 0x0010, 0x0010, 0x0010, 0x0010,
  0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x000B, 0x0010, 0x0010, 0x0010,
  0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
  0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
  0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
  0x000B
};
S_UINT chrominance_ac_code_table[] = {
  0x0000, 0x0001, 0x0004, 0x000A, 0x0018, 0x0019, 0x0038, 0x0078, 0x01F4,
  0x03F6, 0x0FF4, 0x000B, 0x0039, 0x00F6, 0x01F5, 0x07F6, 0x0FF5, 0xFF88,
  0xFF89, 0xFF8A, 0xFF8B, 0x001A, 0x00F7, 0x03F7, 0x0FF6, 0x7FC2, 0xFF8C,
  0xFF8D, 0xFF8E, 0xFF8F, 0xFF90, 0x001B, 0x00F8, 0x03F8, 0x0FF7, 0xFF91,
  0xFF92, 0xFF93, 0xFF94, 0xFF95, 0xFF96, 0x003A, 0x01F6, 0xFF97, 0xFF98,
  0xFF99, 0xFF9A, 0xFF9B, 0xFF9C, 0xFF9D, 0xFF9E, 0x003B, 0x03F9, 0xFF9F,
  0xFFA0, 0xFFA1, 0xFFA2, 0xFFA3, 0xFFA4, 0xFFA5, 0xFFA6, 0x0079, 0x07F7,
  0xFFA7, 0xFFA8, 0xFFA9, 0xFFAA, 0xFFAB, 0xFFAC, 0xFFAD, 0xFFAE, 0x007A,
  0x07F8, 0xFFAF, 0xFFB0, 0xFFB1, 0xFFB2, 0xFFB3, 0xFFB4, 0xFFB5, 0xFFB6,
  0x00F9, 0xFFB7, 0xFFB8, 0xFFB9, 0xFFBA, 0xFFBB, 0xFFBC, 0xFFBD, 0xFFBE,
  0xFFBF, 0x01F7, 0xFFC0, 0xFFC1, 0xFFC2, 0xFFC3, 0xFFC4, 0xFFC5, 0xFFC6,
  0xFFC7, 0xFFC8, 0x01F8, 0xFFC9, 0xFFCA, 0xFFCB, 0xFFCC, 0xFFCD, 0xFFCE,
  0xFFCF, 0xFFD0, 0xFFD1, 0x01F9, 0xFFD2, 0xFFD3, 0xFFD4, 0xFFD5, 0xFFD6,
  0xFFD7, 0xFFD8, 0xFFD9, 0xFFDA, 0x01FA, 0xFFDB, 0xFFDC, 0xFFDD, 0xFFDE,
  0xFFDF, 0xFFE0, 0xFFE1, 0xFFE2, 0xFFE3, 0x07F9, 0xFFE4, 0xFFE5, 0xFFE6,
  0xFFE7, 0xFFE8, 0xFFE9, 0xFFEA, 0xFFEb, 0xFFEC, 0x3FE0, 0xFFED, 0xFFEE,
  0xFFEF, 0xFFF0, 0xFFF1, 0xFFF2, 0xFFF3, 0xFFF4, 0xFFF5, 0x7FC3, 0xFFF6,
  0xFFF7, 0xFFF8, 0xFFF9, 0xFFFA, 0xFFFB, 0xFFFC, 0xFFFD, 0xFFFE,
  0x03FA
};
S_UINT chrominance_ac_size_table[] = {
  0x0002, 0x0002, 0x0003, 0x0004, 0x0005, 0x0005, 0x0006, 0x0007, 0x0009,
  0x000A, 0x000C, 0x0004, 0x0006, 0x0008, 0x0009, 0x000B, 0x000C, 0x0010,
  0x0010, 0x0010, 0x0010, 0x0005, 0x0008, 0x000A, 0x000C, 0x000F, 0x0010,
  0x0010, 0x0010, 0x0010, 0x0010, 0x0005, 0x0008, 0x000A, 0x000C, 0x0010,
  0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0006, 0x0009, 0x0010, 0x0010,
  0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0006, 0x000A, 0x0010,
  0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0007, 0x000B,
  0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0007,
  0x000B, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
  0x0008, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
  0x0010, 0x0009, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
  0x0010, 0x0010, 0x0009, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
  0x0010, 0x0010, 0x0010, 0x0009, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
  0x0010, 0x0010, 0x0010, 0x0010, 0x0009, 0x0010, 0x0010, 0x0010, 0x0010,
  0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x000B, 0x0010, 0x0010, 0x0010,
  0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x000E, 0x0010, 0x0010,
  0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x000F, 0x0010,
  0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
  0x000A
};

S_UINT bitsize[] = {
  0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8
};


static uint16_t markerdata[] = {
  // dht
  0xFFC4, 0x1A2, 0x00,
  // luminance dc (2 - 16) + 1
  0x0105, 0x0101, 0x00101, 0x0101, 0x0000, 0x00000, 00000, 00000,
  // luminance dc (2 - 12) + 1
  0x0102, 0x0304, 0x0506, 0x0708, 0x090A, 0x0B01,
  // chrominance dc (1 - 16)
  0x0003, 0x0101, 0x0101, 0x0101, 0x0101, 0x0100, 0x0000, 0x0000,
  // chrominance dc (1 - 12)
  0x0001, 0x00203, 0x0405, 0x0607, 0x0809, 0x00A0B,
  // luminance ac 1 + (1 - 15)
  0x1000, 0x0201, 0x0303, 0x0204, 0x0305, 0x0504, 0x0400, 0x0001,
  // luminance ac 1 + (1 - 162) + 1
  0x7D01, 0x0203, 0x0004, 0x1105, 0x1221, 0x3141, 0x0613, 0x5161, 0x0722,
  0x7114, 0x3281, 0x91A1, 0x0823, 0x42B1, 0xC115, 0x52D1, 0xF024, 0x3362,
  0x7282, 0x090A, 0x1617, 0x1819, 0x1A25, 0x2627, 0x2829, 0x2A34, 0x3536,
  0x3738, 0x393A, 0x4344, 0x4546, 0x4748, 0x494A, 0x5354, 0x5556, 0x5758,
  0x595A, 0x6364, 0x6566, 0x6768, 0x696A, 0x7374, 0x7576, 0x7778, 0x797A,
  0x8384, 0x8586, 0x8788, 0x898A, 0x9293, 0x9495, 0x9697, 0x9899, 0x9AA2,
  0xA3A4, 0xA5A6, 0xA7A8, 0xA9AA, 0xB2B3, 0xB4B5, 0xB6B7, 0xB8B9, 0xBAC2,
  0xC3C4, 0xC5C6, 0xC7C8, 0xC9CA, 0xD2D3, 0xD4D5, 0xD6D7, 0xD8D9, 0xDAE1,
  0xE2E3, 0xE4E5, 0xE6E7, 0xE8E9, 0xEAF1, 0xF2F3, 0xF4F5, 0xF6F7, 0xF8F9,
  0xFA11,
  // chrominance ac (1 - 16)
  0x0002, 0x0102, 0x0404, 0x0304, 0x0705, 0x0404, 0x0001, 0x0277,
  // chrominance ac (1 - 162)
  0x0001, 0x0203, 0x1104, 0x0521, 0x3106, 0x1241, 0x5107, 0x6171, 0x1322,
  0x3281, 0x0814, 0x4291, 0xA1B1, 0xC109, 0x2333, 0x52F0, 0x1562, 0x72D1,
  0x0A16, 0x2434, 0xE125, 0xF117, 0x1819, 0x1A26, 0x2728, 0x292A, 0x3536,
  0x3738, 0x393A, 0x4344, 0x4546, 0x4748, 0x494A, 0x5354, 0x5556, 0x5758,
  0x595A, 0x6364, 0x6566, 0x6768, 0x696A, 0x7374, 0x7576, 0x7778, 0x797A,
  0x8283, 0x8485, 0x8687, 0x8889, 0x8A92, 0x9394, 0x9596, 0x9798, 0x999A,
  0xA2A3, 0xA4A5, 0xA6A7, 0xA8A9, 0xAAB2, 0xB3B4, 0xB5B6, 0xB7B8, 0xB9BA,
  0xC2C3, 0xC4C5, 0xC6C7, 0xC8C9, 0xCAD2, 0xD3D4, 0xD5D6, 0xD7D8, 0xD9DA,
  0xE2E3, 0xE4E5, 0xE6E7, 0xE8E9, 0xEAF2, 0xF3F4, 0xF5F6, 0xF7F8, 0xF9FA
};


static S_UINT zigzag_table[] = {
  0, 1, 5, 6, 14, 15, 27, 28, 2, 4, 7, 13, 16, 26, 29, 42, 3, 8, 12, 17,
  25, 30, 41, 43, 9, 11, 18, 24, 31, 40, 44, 53, 10, 19, 23, 32, 39, 45, 52,
  54, 20, 22, 33, 38, 46, 51, 55, 60, 21, 34, 37, 47, 50, 56, 59, 61, 35, 36,
  48, 49, 57, 58, 62, 63
};



/*  This function implements 16 Step division for Q.15 format data */
/*  and taking integer part */
INLINE  S_UINT Q15_Division_Integer (uint32_t numer, uint32_t denom) 
{
	/*
  int i;
  denom <<= 15;
  for (i = 16; i > 0; i--)
	{
		if (numer > denom)
		{
			numer -= denom;
			numer <<= 1;
			numer++;
		} else 
			numer <<= 1;
	}
  return (uint16_t) numer;
	*/
	
	//numer<<=15;
	numer/=denom;
	return (S_UINT) numer;
}

/* Multiply Quantization table with quality factor to get LQT and CQT */
INLINE void initialize_quantization_tables(S_JPEG_ENCODER_STRUCTURE * jpeg,uint32_t quality_factor)
{
  S_INT i, index;
  uint32_t value;
  
  uint8_t luminance_quant_table[] = {
    16, 11, 10, 16, 24, 40, 51, 61, 12, 12, 14, 19, 26, 58, 60, 55, 14, 13,
    16, 24, 40, 57, 69, 56, 14, 17, 22, 29, 51, 87, 80, 62, 18, 22, 37,
    56, 68, 109, 103, 77, 24, 35, 55, 64, 81, 104, 113, 92, 49, 64, 78,
    87, 103, 121, 120, 101, 72, 92, 95, 98, 112, 100, 103, 99
  };
  
  uint8_t chrominance_quant_table[] = {
    17, 18, 24, 47, 99, 99, 99, 99, 18, 21, 26, 66, 99, 99, 99, 99, 24, 26,
    56, 99, 99, 99, 99, 99, 47, 66, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99
  };

  for (i = 0; i < S_BLOCK_SIZE; i++)
  {
    index = zigzag_table[i];

    /* luminance quantization table * quality factor */
    value = luminance_quant_table[i] * quality_factor;
    value = (value + 0x200) >> 10;

    if (value == 0)
      value = 1;

    else if (value > 255)
      value = 255;

    jpeg->Lqt[index] = (uint8_t) value;

    jpeg->ILqt[i] = (S_UINT)Q15_Division_Integer(0x8000, value);

    /* chrominance quantization table * quality factor */
    value = chrominance_quant_table[i] * quality_factor;

    value = (value + 0x200) >> 10;

    if (value == 0)
      value = 1;

    else if (value > 255)
      value = 255;

    jpeg->Cqt[index] = (uint8_t) value;

    jpeg->ICqt[i] = (S_UINT)Q15_Division_Integer(0x8000, value);
  }
}

/* multiply DCT Coefficients with Quantization table and store in ZigZag location */
INLINE void quantization(S_JPEG_ENCODER_STRUCTURE * jpeg, S_INT* data, const S_UINT* quant_table)
{
  S_UINT i;
  int32_t value;

  for (i = 0; i <S_BLOCK_SIZE; i++)
  {
    value = data[i] * quant_table[i];
    value = (value + 0x4000) >> 15;
    jpeg->Temp[zigzag_table[i]] = (S_INT) value;
  }
}


static void initialization(S_JPEG_ENCODER_STRUCTURE * jpeg, 
                           uint32_t image_format,
                           uint32_t image_width, uint32_t image_height)
{
  S_UINT mcu_width=8, mcu_height=8, bytes_per_pixel=1;
  jpeg->lcode = 0;
  jpeg->bitindex = 0;

	
	switch(image_format)
	{
		case FORMAT_CbCr400:
			jpeg->mcu_width = mcu_width = 8;
			jpeg->mcu_height = mcu_height = 8;
			jpeg->horizontal_mcus = (S_UINT)((image_width + mcu_width - 1) >> 3);
			jpeg->vertical_mcus = (S_UINT)((image_height + mcu_height - 1) >> 3);
			bytes_per_pixel = 1;
			jpeg->read_format = read_YCbCr400;
			break;
			
		case FORMAT_CbCr444:
			jpeg->mcu_width = mcu_width = 8;
			jpeg->mcu_height = mcu_height = 8;
			jpeg->horizontal_mcus = (S_UINT)((image_width + mcu_width - 1) >> 3);
			jpeg->vertical_mcus = (S_UINT)((image_height + mcu_height - 1) >> 3);
      bytes_per_pixel = 3;
      jpeg->read_format = read_YCbCr444;
			break;
			
		case FORMAT_CbCr420:
			jpeg->mcu_width       = mcu_width = 16;
			jpeg->horizontal_mcus = (S_UINT)((image_width + mcu_width - 1) >> 4);
      jpeg->mcu_height      = mcu_height = 16;
      jpeg->vertical_mcus   = (S_UINT)((image_height + mcu_height - 1) >> 4);
      bytes_per_pixel = 3;
      jpeg->read_format = read_YCbCr420;
			break;
			
		case FORMAT_CbCr420p://TODO: finish this
			jpeg->mcu_width       = mcu_width = 16;
			jpeg->horizontal_mcus = (S_UINT)((image_width + mcu_width - 1) >> 4);
      jpeg->mcu_height      = mcu_height = 16;
      jpeg->vertical_mcus   = (S_UINT)((image_height + mcu_height - 1) >> 4);
      bytes_per_pixel = 3;
      jpeg->read_format = read_YCbCr420p; 
			break;
			
		case FORMAT_CbCr422:
			jpeg->mcu_width       = mcu_width = 16;
			jpeg->horizontal_mcus = (S_UINT)((image_width + mcu_width - 1) >> 4);

      jpeg->mcu_height = mcu_height = 8;
      jpeg->vertical_mcus =  (S_UINT)((image_height + mcu_height - 1) >> 3);
      bytes_per_pixel = 2;
      jpeg->read_format = read_YCbCr422;
			break;
			
		case FORMAT_CbCr422p:
			//printf("Format CbCr422p\n");
			jpeg->mcu_width       = mcu_width = 16;
			jpeg->horizontal_mcus = (S_UINT)((image_width + mcu_width - 1) >> 4);

      jpeg->mcu_height = mcu_height = 8;
      jpeg->vertical_mcus =   (S_UINT)((image_height + mcu_height - 1) >> 3);
      bytes_per_pixel = 1;
      jpeg->read_format = read_YCbCr422p;
			
			jpeg->CbOffset= image_width*image_height;
			jpeg->CrOffset= image_width*image_height+image_width*image_height/2;
			
			break;
		
  }

  jpeg->rows_in_bottom_mcus =    (S_UINT)(image_height - (jpeg->vertical_mcus - 1) * mcu_height);
	jpeg->cols_in_right_mcus =     (S_UINT)(image_width -  (jpeg->horizontal_mcus - 1) * mcu_width);
	
  jpeg->length_minus_mcu_width = (S_UINT)((image_width - mcu_width) * bytes_per_pixel);
  jpeg->length_minus_width =     (S_UINT)((image_width - jpeg->cols_in_right_mcus) * bytes_per_pixel);
	
  jpeg->mcu_width_size  =        (S_UINT)(mcu_width * bytes_per_pixel);

  if (image_format == FORMAT_CbCr420 || image_format == FORMAT_CbCr420p)
	{
    jpeg->mcu_line_offset = (S_UINT)((image_width * ((mcu_height >> 1) - 1) -
                   (mcu_width - jpeg->cols_in_right_mcus)) * bytes_per_pixel);
									 
		jpeg->mcu_height_size =  (S_UINT)(image_width * mcu_height/2 * bytes_per_pixel);
	}
	else
	{
    jpeg->mcu_line_offset = (S_UINT)((image_width * (mcu_height - 1) -
                   (mcu_width - jpeg->cols_in_right_mcus)) * bytes_per_pixel);
									 
		jpeg->mcu_height_size =  (S_UINT)(image_width * mcu_height * bytes_per_pixel);
		
	}
	//printf("mcu_height_size=%d,mcu_width_size=%d\n",jpeg->mcu_height_size,jpeg->mcu_width_size);

  jpeg->ldc1 = 0;

  jpeg->ldc2 = 0;

  jpeg->ldc3 = 0;
}

uint32_t s_encode_image(uint8_t * input_ptr, uint8_t * output_ptr,
                        uint32_t quality_factor, int image_format,
                        uint32_t image_width, uint32_t image_height,
                        uint32_t output_buffer_size)
{
  S_UINT i, j;
  S_UINT last_col;
	S_UINT last_row;

  uint8_t * output;
	
  S_JPEG_ENCODER_STRUCTURE JpegStruct;
  S_JPEG_ENCODER_STRUCTURE * enc = &JpegStruct;
	
	/*(S_JPEG_ENCODER_STRUCTURE *)malloc(sizeof(S_JPEG_ENCODER_STRUCTURE));
	memset(enc,0,sizeof(S_JPEG_ENCODER_STRUCTURE));*/
	
  output = output_ptr;

	/* Initialization of JPEG control structure */
  initialization(enc, image_format, image_width, image_height);

  /* Quantization Table Initialization */
  initialize_quantization_tables(enc,quality_factor);

  /* Writing Marker Data */
  output_ptr = write_markers(enc,output_ptr, image_format, image_width, image_height);
	
	last_row=enc->vertical_mcus-1;
	last_col=enc->horizontal_mcus-1;
	
  for (i = 0; i < enc->vertical_mcus; i++)
  {
    if (i < last_row)
      enc->rows = enc->mcu_height;
    else
      enc->rows = enc->rows_in_bottom_mcus;

    for (j = 0; j < enc->horizontal_mcus; j++)
    {
      if (j < last_col)
      {
        enc->cols = enc->mcu_width;
        enc->scan_line_incr = enc->length_minus_mcu_width;
      } else {
        enc->cols = enc->cols_in_right_mcus;
        enc->scan_line_incr = enc->length_minus_width;
      }

      enc->read_format(enc, input_ptr,i,j);

      /* Encode the data in MCU */
      output_ptr = encodeMCU(enc, image_format, output_ptr);
      //input_ptr += enc->mcu_width_size;
    }
    //input_ptr += enc->mcu_line_offset;
  }

  /* Close Routine */
  output_ptr = close_bitstream(enc,output_ptr);
  //free(enc);
	
  return (uint32_t)(output_ptr - output);
}

static uint8_t *encodeMCU(S_JPEG_ENCODER_STRUCTURE * enc,
          uint32_t image_format, uint8_t * output_ptr)
{
  DCT(enc->Y1);
  quantization(enc,enc->Y1, enc->ILqt);
	
  output_ptr = huffman(enc, COMPONENT_Y, output_ptr);

  if (image_format == FORMAT_CbCr400)
    return output_ptr;

  DCT(enc->Y2);

  quantization(enc,enc->Y2, enc->ILqt);

  output_ptr = huffman(enc, COMPONENT_Y, output_ptr);

  if (image_format == FORMAT_CbCr422 || image_format == FORMAT_CbCr422p)
    goto chroma;

  DCT(enc->Y3);

  quantization(enc,enc->Y3, enc->ILqt);

  output_ptr = huffman(enc, COMPONENT_Y, output_ptr);

  DCT(enc->Y4);

  quantization(enc,enc->Y4, enc->ILqt);

  output_ptr = huffman(enc, COMPONENT_Y, output_ptr);

chroma:DCT(enc->CB);

  quantization(enc,enc->CB, enc->ICqt);

  output_ptr = huffman(enc, COMPONENT_CB, output_ptr);

  DCT(enc->CR);

  quantization(enc,enc->CR, enc->ICqt);

  output_ptr = huffman(enc, COMPONENT_CR, output_ptr);

  return output_ptr;
}

 
/* DCT for One block(8x8) */
static void DCT(S_INT * data)
{
  S_UINT i;
  int32_t x0, x1, x2, x3, x4, x5, x6, x7, x8;

  /*  All values are shifted left by 10
    and rounded off to nearest integer */
  static const uint16_t c1 = 1420;  /* cos PI/16 * root(2)  */
  static const uint16_t c2 = 1338;  /* cos PI/8 * root(2)   */
  static const uint16_t c3 = 1204;  /* cos 3PI/16 * root(2) */
  static const uint16_t c5 = 805;   /* cos 5PI/16 * root(2) */
  static const uint16_t c6 = 554;   /* cos 3PI/8 * root(2)  */
  static const uint16_t c7 = 283;   /* cos 7PI/16 * root(2) */
  static const uint16_t s1 = 3;
  static const uint16_t s2 = 10;
  static const uint16_t s3 = 13;

  for (i = 0; i < 8; i++)
  {
    x8 = data[0] + data[7];
    x0 = data[0] - data[7];
    x7 = data[1] + data[6];
    x1 = data[1] - data[6];
    x6 = data[2] + data[5];
    x2 = data[2] - data[5];
    x5 = data[3] + data[4];
    x3 = data[3] - data[4];
    x4 = x8 + x5;
    x8 -= x5;
    x5 = x7 + x6;
    x7 -= x6;
    data[0] = (S_INT)(x4 + x5);
    data[4] = (S_INT)(x4 - x5);
    data[2] = (S_INT)((x8 * c2 + x7 * c6) >> s2);
    data[6] = (S_INT)((x8 * c6 - x7 * c2) >> s2);
    data[7] = (S_INT)((x0 * c7 - x1 * c5 + x2 * c3 - x3 * c1) >> s2);
    data[5] = (S_INT)((x0 * c5 - x1 * c1 + x2 * c7 + x3 * c3) >> s2);
    data[3] = (S_INT)((x0 * c3 - x1 * c7 - x2 * c1 - x3 * c5) >> s2);
    data[1] = (S_INT)((x0 * c1 + x1 * c3 + x2 * c5 + x3 * c7) >> s2);
    data += 8;
  }

  data -= 64;

  for (i = 0; i <8; i++)
  {
    x8 = data[0] + data[56];
    x0 = data[0] - data[56];
    x7 = data[8] + data[48];
    x1 = data[8] - data[48];
    x6 = data[16] + data[40];
    x2 = data[16] - data[40];
    x5 = data[24] + data[32];
    x3 = data[24] - data[32];
    x4 = x8 + x5;
    x8 -= x5;
    x5 = x7 + x6;
    x7 -= x6;
    data[0] = (S_INT)((x4 + x5) >> s1);
    data[32] = (S_INT)((x4 - x5) >> s1);
    data[16] = (S_INT)((x8 * c2 + x7 * c6) >> s3);
    data[48] = (S_INT)((x8 * c6 - x7 * c2) >> s3);
    data[56] = (S_INT)((x0 * c7 - x1 * c5 + x2 * c3 - x3 * c1) >> s3);
    data[40] = (S_INT)((x0 * c5 - x1 * c1 + x2 * c7 + x3 * c3) >> s3);
    data[24] = (S_INT)((x0 * c3 - x1 * c7 - x2 * c1 - x3 * c5) >> s3);
    data[8] =  (S_INT)((x0 * c1 + x1 * c3 + x2 * c5 + x3 * c7) >> s3);
    data++;
  }
}

static void read_YCbCr400(S_JPEG_ENCODER_STRUCTURE * enc, uint8_t * input_ptr_,S_UINT row,S_UINT col)
{
	uint8_t * input_ptr=input_ptr_+row*enc->mcu_height_size+col*enc->mcu_width_size;
	
  S_INT i, j;
  S_INT *Y1_Ptr = enc->Y1;
  S_UINT rows =  enc->rows;
  S_UINT cols =  enc->cols;
  S_UINT scan_line_incr =  enc->scan_line_incr;

  for (i = rows; i > 0; i--)
  {
    for (j = cols; j > 0; j--)
      *Y1_Ptr++ = *input_ptr++ - 128;

    for (j = 8 - cols; j > 0; j--)
      {*Y1_Ptr = *(Y1_Ptr - 1);Y1_Ptr++;}

    input_ptr += scan_line_incr;
  }

  for (i = 8 - rows; i > 0; i--)
  {
    for (j = 8; j > 0; j--)
      {*Y1_Ptr = *(Y1_Ptr - 8); Y1_Ptr++;}
  }
}

static void read_YCbCr420(S_JPEG_ENCODER_STRUCTURE * enc, uint8_t * input_ptr_,S_UINT row,S_UINT col)
{
	uint8_t * input_ptr=input_ptr_+ row*enc->mcu_height_size + col*enc->mcu_width_size;
	//printf("read_YCbCr420: %p \n",input_ptr);
  S_UINT i, j;
  S_UINT Y1_rows, Y3_rows, Y1_cols, Y2_cols;
  S_INT * Y1_Ptr = enc->Y1;
  S_INT * Y2_Ptr = enc->Y2;
  S_INT * Y3_Ptr = enc->Y3;
  S_INT * Y4_Ptr = enc->Y4;
  S_INT * CB_Ptr = enc->CB;
  S_INT * CR_Ptr = enc->CR;
  S_INT * Y1Ptr  = enc->Y1 + 8;
  S_INT * Y2Ptr  = enc->Y2 + 8;
  S_INT * Y3Ptr  = enc->Y3 + 8;
  S_INT * Y4Ptr  = enc->Y4 + 8;
  S_UINT rows  = enc->rows;
  S_UINT cols  = enc->cols;
  S_UINT scan_line_incr  = enc->scan_line_incr;

  if (rows <= 8)
  {
    Y1_rows = rows;
    Y3_rows = 0;
  } else {
    Y1_rows = 8;
    Y3_rows = (S_UINT)(rows - 8);
  }

  if (cols <= 8)
  {
    Y1_cols = cols;
    Y2_cols = 0;
  } else {
    Y1_cols = 8;
    Y2_cols = (S_UINT)(cols - 8);
  }

  for (i = Y1_rows >> 1; i > 0; i--)
  {
    for (j = Y1_cols >> 1; j > 0; j--)
    {
      *Y1_Ptr++ = *input_ptr++ - 128;
      *Y1_Ptr++ = *input_ptr++ - 128;
      *Y1Ptr++ = *input_ptr++ - 128;
      *Y1Ptr++ = *input_ptr++ - 128;
      *CB_Ptr++ = *input_ptr++ - 128;
      *CR_Ptr++ = *input_ptr++ - 128;
    }
    
    for (j = Y2_cols >> 1; j > 0; j--)
    {
      *Y2_Ptr++ = *input_ptr++ - 128;
      *Y2_Ptr++ = *input_ptr++ - 128;
      *Y2Ptr++ = *input_ptr++ - 128;
      *Y2Ptr++ = *input_ptr++ - 128;
      *CB_Ptr++ = *input_ptr++ - 128;
      *CR_Ptr++ = *input_ptr++ - 128;
    }

    if (cols <= 8)
    {
      for (j = 8 - Y1_cols; j > 0; j--)
      {
        *Y1_Ptr = *(Y1_Ptr - 1);
				Y1_Ptr++;
        *Y1Ptr = *(Y1Ptr - 1);
				Y1Ptr++;
      }
      for (j = 8; j > 0; j--)
      {
        *Y2_Ptr++ = *(Y1_Ptr - 1);
        *Y2Ptr++ = *(Y1Ptr - 1);
      }
    } else {
      for (j = 8 - Y2_cols; j > 0; j--)
      {
        *Y2_Ptr = *(Y2_Ptr - 1);
				Y2_Ptr++;
        *Y2Ptr = *(Y2Ptr - 1);
				Y2Ptr++;
      }
    }

    for (j = (16 - cols) >> 1; j > 0; j--)
    {
      *CB_Ptr = *(CB_Ptr - 1);
			CB_Ptr++;
      *CR_Ptr = *(CR_Ptr - 1);
			CR_Ptr++;
    }
    Y1_Ptr += 8;

    Y2_Ptr += 8;
    Y1Ptr += 8;
    Y2Ptr += 8;
    input_ptr += scan_line_incr;
  }

  for (i = Y3_rows >> 1; i > 0; i--)
  {
    for (j = Y1_cols >> 1; j > 0; j--)
    {
      *Y3_Ptr++ = *input_ptr++ - 128;
      *Y3_Ptr++ = *input_ptr++ - 128;
      *Y3Ptr++ = *input_ptr++ - 128;
      *Y3Ptr++ = *input_ptr++ - 128;
      *CB_Ptr++ = *input_ptr++ - 128;
      *CR_Ptr++ = *input_ptr++ - 128;
    }
    for (j = Y2_cols >> 1; j > 0; j--)
    {
      *Y4_Ptr++ = *input_ptr++ - 128;
      *Y4_Ptr++ = *input_ptr++ - 128;
      *Y4Ptr++ = *input_ptr++ - 128;
      *Y4Ptr++ = *input_ptr++ - 128;
      *CB_Ptr++ = *input_ptr++ - 128;
      *CR_Ptr++ = *input_ptr++ - 128;
    }

    if (cols <= 8)

    {
      for (j = 8 - Y1_cols; j > 0; j--)
      {
        *Y3_Ptr = *(Y3_Ptr - 1);
				Y3_Ptr++;
				
        *Y3Ptr = *(Y3Ptr - 1);
				Y3Ptr++;
      }

      for (j = 8; j > 0; j--)
      {
        *Y4_Ptr++ = *(Y3_Ptr - 1);
        *Y4Ptr++ = *(Y3Ptr - 1);
      }
    } else {
      for (j = 8 - Y2_cols; j > 0; j--)

      {
        *Y4_Ptr = *(Y4_Ptr - 1);
				Y4_Ptr++;
        *Y4Ptr = *(Y4Ptr - 1);
				Y4Ptr++;
      }
    }

    for (j = (16 - cols) >> 1; j > 0; j--)
    {
      *CB_Ptr = *(CB_Ptr - 1);
			CB_Ptr++;
      *CR_Ptr = *(CR_Ptr - 1);
			CR_Ptr++;
    }

    Y3_Ptr += 8;

    Y4_Ptr += 8;
    Y3Ptr += 8;
    Y4Ptr += 8;
    input_ptr += scan_line_incr;
  }

  if (rows <= 8)
  {
    for (i = 8 - rows; i > 0; i--)
    {
      for (j = 8; j > 0; j--)
      {
        *Y1_Ptr = *(Y1_Ptr - 8);
				Y1_Ptr++;
        *Y2_Ptr = *(Y2_Ptr - 8);
				Y2_Ptr++;
      }
    }
    
    for (i = 8; i > 0; i--)
    {
      Y1_Ptr -= 8;
      Y2_Ptr -= 8;

      for (j = 8; j > 0; j--)
      {
        *Y3_Ptr++ = *Y1_Ptr++;
        *Y4_Ptr++ = *Y2_Ptr++;
      }
    }
  } else {
    for (i = (16 - rows); i > 0; i--)
    {
      for (j = 8; j > 0; j--)

      {
        *Y3_Ptr = *(Y3_Ptr - 8);
				Y3_Ptr++;
        *Y4_Ptr = *(Y4_Ptr - 8);
				Y4_Ptr++;
      }
    }
  }

  for (i = ((16 - rows) >> 1); i > 0; i--)
  {
    for (j = 8; j > 0; j--)
    {
      *CB_Ptr = *(CB_Ptr - 8);
			CB_Ptr++;
      *CR_Ptr = *(CR_Ptr - 8);
			CR_Ptr++;
    }
  }
}

static void read_YCbCr420p(S_JPEG_ENCODER_STRUCTURE * enc, uint8_t * input_ptr_,S_UINT row,S_UINT col)//TODO: make this work
{
	uint8_t * input_ptr=input_ptr_+row*enc->mcu_height_size+col*enc->mcu_width_size;
	
  S_UINT i, j;
  S_UINT Y1_rows, Y3_rows, Y1_cols, Y2_cols;
  S_INT * Y1_Ptr = enc->Y1;
  S_INT * Y2_Ptr = enc->Y2;
  S_INT * Y3_Ptr = enc->Y3;
  S_INT * Y4_Ptr = enc->Y4;
  S_INT * CB_Ptr = enc->CB;
  S_INT * CR_Ptr = enc->CR;
  S_INT * Y1Ptr  = enc->Y1 + 8;
  S_INT * Y2Ptr  = enc->Y2 + 8;
  S_INT * Y3Ptr  = enc->Y3 + 8;
  S_INT * Y4Ptr  = enc->Y4 + 8;
  S_UINT rows  = enc->rows;
  S_UINT cols  = enc->cols;
  S_UINT scan_line_incr  = enc->scan_line_incr;

  if (rows <= 8)
  {
    Y1_rows = rows;
    Y3_rows = 0;
  } else {
    Y1_rows = 8;
    Y3_rows = (uint16_t)(rows - 8);
  }

  if (cols <= 8)
  {
    Y1_cols = cols;
    Y2_cols = 0;
  } else {
    Y1_cols = 8;
    Y2_cols = (uint16_t)(cols - 8);
  }

  for (i = Y1_rows >> 1; i > 0; i--)
  {
    for (j = Y1_cols >> 1; j > 0; j--)
    {
      *Y1_Ptr++ = *input_ptr++ - 128;
      *Y1_Ptr++ = *input_ptr++ - 128;
      *Y1Ptr++ = *input_ptr++ - 128;
      *Y1Ptr++ = *input_ptr++ - 128;
      *CB_Ptr++ = *input_ptr++ - 128;
      *CR_Ptr++ = *input_ptr++ - 128;
    }
    
    for (j = Y2_cols >> 1; j > 0; j--)
    {
      *Y2_Ptr++ = *input_ptr++ - 128;
      *Y2_Ptr++ = *input_ptr++ - 128;
      *Y2Ptr++ = *input_ptr++ - 128;
      *Y2Ptr++ = *input_ptr++ - 128;
      *CB_Ptr++ = *input_ptr++ - 128;
      *CR_Ptr++ = *input_ptr++ - 128;
    }

    if (cols <= 8)
    {
      for (j = 8 - Y1_cols; j > 0; j--)
      {
        *Y1_Ptr = *(Y1_Ptr - 1);Y1_Ptr++;
        *Y1Ptr = *(Y1Ptr - 1);Y1Ptr++;
      }
      for (j = 8; j > 0; j--)
      {
        *Y2_Ptr++ = *(Y1_Ptr - 1);
        *Y2Ptr++ = *(Y1Ptr - 1);
      }
    } else {
      for (j = 8 - Y2_cols; j > 0; j--)
      {
        *Y2_Ptr = *(Y2_Ptr - 1);Y2_Ptr++;
        *Y2Ptr = *(Y2Ptr - 1);Y2Ptr++;
      }
    }

    for (j = (16 - cols) >> 1; j > 0; j--)
    {
      *CB_Ptr = *(CB_Ptr - 1);CB_Ptr++;
      *CR_Ptr = *(CR_Ptr - 1);CR_Ptr++;
    }
    Y1_Ptr += 8;

    Y2_Ptr += 8;
    Y1Ptr += 8;
    Y2Ptr += 8;
    input_ptr += scan_line_incr;
  }

  for (i = Y3_rows >> 1; i > 0; i--)
  {
    for (j = Y1_cols >> 1; j > 0; j--)
    {
      *Y3_Ptr++ = *input_ptr++ - 128;
      *Y3_Ptr++ = *input_ptr++ - 128;
      *Y3Ptr++ = *input_ptr++ - 128;
      *Y3Ptr++ = *input_ptr++ - 128;
      *CB_Ptr++ = *input_ptr++ - 128;
      *CR_Ptr++ = *input_ptr++ - 128;
    }
    for (j = Y2_cols >> 1; j > 0; j--)
    {
      *Y4_Ptr++ = *input_ptr++ - 128;
      *Y4_Ptr++ = *input_ptr++ - 128;
      *Y4Ptr++ = *input_ptr++ - 128;
      *Y4Ptr++ = *input_ptr++ - 128;
      *CB_Ptr++ = *input_ptr++ - 128;
      *CR_Ptr++ = *input_ptr++ - 128;
    }

    if (cols <= 8)
    {
      for (j = 8 - Y1_cols; j > 0; j--)

      {
        *Y3_Ptr = *(Y3_Ptr - 1);Y3_Ptr++;
        *Y3Ptr = *(Y3Ptr - 1);Y3Ptr++;
      }

      for (j = 8; j > 0; j--)

      {
        *Y4_Ptr++ = *(Y3_Ptr - 1);
        *Y4Ptr++ = *(Y3Ptr - 1);
      }
    }
    else
    {
      for (j = 8 - Y2_cols; j > 0; j--)

      {
        *Y4_Ptr = *(Y4_Ptr - 1);Y4_Ptr++;
        *Y4Ptr = *(Y4Ptr - 1);Y4Ptr++;
      }
    }

    for (j = (16 - cols) >> 1; j > 0; j--)

    {
      *CB_Ptr = *(CB_Ptr - 1);CB_Ptr++;
      *CR_Ptr = *(CR_Ptr - 1);CR_Ptr++;
    }

    Y3_Ptr += 8;

    Y4_Ptr += 8;
    Y3Ptr += 8;
    Y4Ptr += 8;
    input_ptr += scan_line_incr;
  }

  if (rows <= 8)
  {
    for (i = 8 - rows; i > 0; i--)
    {
      for (j = 8; j > 0; j--)
      {
        *Y1_Ptr = *(Y1_Ptr - 8);Y1_Ptr++;
        *Y2_Ptr = *(Y2_Ptr - 8);Y2_Ptr++;
      }
    }
    
    for (i = 8; i > 0; i--)
    {
      Y1_Ptr -= 8;
      Y2_Ptr -= 8;

      for (j = 8; j > 0; j--)
      {
        *Y3_Ptr++ = *Y1_Ptr++;
        *Y4_Ptr++ = *Y2_Ptr++;
      }
    }
  } else {
    for (i = (16 - rows); i > 0; i--)

    {
      for (j = 8; j > 0; j--)

      {
        *Y3_Ptr = *(Y3_Ptr - 8);Y3_Ptr++;
        *Y4_Ptr = *(Y4_Ptr - 8);Y4_Ptr++;
      }
    }
  }

  for (i = ((16 - rows) >> 1); i > 0; i--)
  {
    for (j = 8; j > 0; j--)

    {
      *CB_Ptr = *(CB_Ptr - 8);CB_Ptr++;
      *CR_Ptr = *(CR_Ptr - 8);CR_Ptr++;
    }
  }
}


static void read_YCbCr422(S_JPEG_ENCODER_STRUCTURE * enc,uint8_t * input_ptr_,S_UINT row,S_UINT col)
{
	uint8_t * input_ptr=input_ptr_+row*enc->mcu_height_size+col*enc->mcu_width_size;
	
  S_INT i, j;
  S_UINT Y1_cols, Y2_cols;
	
  S_INT * Y1_Ptr = enc->Y1;
  S_INT * Y2_Ptr = enc->Y2;
	
  S_INT * CB_Ptr = enc->CB;
  S_INT * CR_Ptr = enc->CR;
	
  S_UINT rows = enc->rows;
  S_UINT cols = enc->cols;
	
  S_UINT scan_line_incr = enc->scan_line_incr;
  
  if (cols <= 8)
  {
    Y1_cols = cols;
    Y2_cols = 0;
  } else  {
    Y1_cols = 8;
    Y2_cols = (S_UINT)(cols - 8);
  }

  for (i = rows; i > 0; i--)
  {
    for (j = Y1_cols >> 1; j > 0; j--)
    {
      *Y1_Ptr++ = *input_ptr++ - 128;
      *CB_Ptr++ = *input_ptr++ - 128;
      *Y1_Ptr++ = *input_ptr++ - 128;
      *CR_Ptr++ = *input_ptr++ - 128;
    }

    for (j = Y2_cols >> 1; j > 0; j--)
    {
      *Y2_Ptr++ = *input_ptr++ - 128;
      *CB_Ptr++ = *input_ptr++ - 128;
      *Y2_Ptr++ = *input_ptr++ - 128;
      *CR_Ptr++ = *input_ptr++ - 128;
    }

    if (cols <= 8) // fill edge
    {
      for (j = 8 - Y1_cols; j > 0; j--) 
        {*Y1_Ptr = *(Y1_Ptr - 1);Y1_Ptr++;}

      for (j = 8 - Y2_cols; j > 0; j--)
        {*Y2_Ptr = *(Y1_Ptr - 1);Y2_Ptr++;}
      
    } else {
      for (j = 8 - Y2_cols; j > 0; j--)
        {*Y2_Ptr = *(Y2_Ptr - 1);Y2_Ptr++;}
    }

    for (j = (16 - cols) >> 1; j > 0; j--) //fill edge
    {
      *CB_Ptr = *(CB_Ptr - 1); 
			CB_Ptr++;
      *CR_Ptr = *(CR_Ptr - 1); 
			CR_Ptr++;
    }
    input_ptr += scan_line_incr;
  }

  for (i = 8 - rows; i > 0; i--) //fill edge
  {
    for (j = 8; j > 0; j--)
    {
      *Y1_Ptr = *(Y1_Ptr - 8);
			Y1_Ptr++;
      *Y2_Ptr = *(Y2_Ptr - 8);
			Y2_Ptr++;
      *CB_Ptr = *(CB_Ptr - 8);
			CB_Ptr++;
      *CR_Ptr = *(CR_Ptr - 8);
			CR_Ptr++;
    }
  }
}

static void read_YCbCr422p(S_JPEG_ENCODER_STRUCTURE * enc,uint8_t * input_ptr_,S_UINT row,S_UINT col)
{
	uint8_t * input_ptr=input_ptr_+ row*enc->mcu_height_size + col*enc->mcu_width_size;
	
	uint8_t * input_ptr_Cb=input_ptr_+enc->CbOffset+(row*enc->mcu_height_size+col*enc->mcu_width_size)/2;
	uint8_t * input_ptr_Cr=input_ptr_+enc->CrOffset+(row*enc->mcu_height_size+col*enc->mcu_width_size)/2;
	
  S_INT i, j;
  S_UINT Y1_cols, Y2_cols;
	
  S_INT * Y1_Ptr = enc->Y1;
  S_INT * Y2_Ptr = enc->Y2;
	
  S_INT * CB_Ptr = enc->CB;
  S_INT * CR_Ptr = enc->CR;
	
  S_UINT rows = enc->rows;
  S_UINT cols = enc->cols;
	
  uint32_t  scan_line_incr = enc->scan_line_incr;
	uint32_t  scan_line_incr_CbCr = enc->scan_line_incr/2;
	
  //printf("Input ptr=%p\n",input_ptr);
	
  if (cols <= 8)
  {
    Y1_cols = cols;
    Y2_cols = 0;
  } else  {
    Y1_cols = 8;
    Y2_cols = (S_UINT)(cols - 8);
  }

  for (i = rows; i > 0; i--)
  {
    for (j = Y1_cols >> 1; j > 0; j--)
    {
      *Y1_Ptr++ = *input_ptr++ - 128;
      *CB_Ptr++ = *input_ptr_Cb++ - 128;
			
      *Y1_Ptr++ = *input_ptr++ - 128;
      *CR_Ptr++ = *input_ptr_Cr++ - 128;
    }

    for (j = Y2_cols >> 1; j > 0; j--)
    {
      *Y2_Ptr++ = *input_ptr++ - 128;
      *CB_Ptr++ = *input_ptr_Cb++ - 128;
			
      *Y2_Ptr++ = *input_ptr++ - 128;
      *CR_Ptr++ = *input_ptr_Cr++ - 128;
    }

    if (cols <= 8) // fill edge
    {
      for (j = 8 - Y1_cols; j > 0; j--) 
        {*Y1_Ptr = *(Y1_Ptr - 1);Y1_Ptr++;}

      for (j = 8 - Y2_cols; j > 0; j--)
        {*Y2_Ptr++ = *(Y1_Ptr - 1);}
      
    } else {
      for (j = 8 - Y2_cols; j > 0; j--)
        {*Y2_Ptr = *(Y2_Ptr - 1);Y2_Ptr++;}
    }

    for (j = (16 - cols) >> 1; j > 0; j--) //fill edge
    {
      *CB_Ptr = *(CB_Ptr - 1); CB_Ptr++;
      *CR_Ptr = *(CR_Ptr - 1); CR_Ptr++;
    }

    input_ptr += scan_line_incr;
		input_ptr_Cb+=scan_line_incr_CbCr;
		input_ptr_Cr+=scan_line_incr_CbCr;
  }

  for (i = 8 - rows; i > 0; i--) //fill edge
  {
    for (j = 8; j > 0; j--)
    {
     
      *Y1_Ptr = *(Y1_Ptr - 8);Y1_Ptr++;
      *Y2_Ptr = *(Y2_Ptr - 8);Y2_Ptr++;
      *CB_Ptr = *(CB_Ptr - 8);CB_Ptr++;
      *CR_Ptr = *(CR_Ptr - 8);CR_Ptr++;
    }
  }
}

static void read_YCbCr444(S_JPEG_ENCODER_STRUCTURE * enc, uint8_t * input_ptr_,S_UINT row,S_UINT col)
{
	uint8_t * input_ptr=input_ptr_+row*enc->mcu_height_size+col*enc->mcu_width_size;	
  S_INT i, j;
  S_INT * Y1_Ptr = enc->Y1;
  S_INT * CB_Ptr = enc->CB;
  S_INT * CR_Ptr = enc->CR;
  S_UINT rows = enc->rows;
  S_UINT cols = enc->cols;
  S_UINT scan_line_incr = enc->scan_line_incr;

  for (i = rows; i > 0; i--)
  {
    for (j = cols; j > 0; j--)
    {
      *Y1_Ptr++ = *input_ptr++ - 128;
      *CB_Ptr++ = *input_ptr++ - 128;
      *CR_Ptr++ = *input_ptr++ - 128;
    }

    for (j = 8 - cols; j > 0; j--)
    {
      *Y1_Ptr = *(Y1_Ptr - 1);Y1_Ptr++;
      *CB_Ptr = *(CB_Ptr - 1);CB_Ptr++;
      *CR_Ptr = *(CR_Ptr - 1);CR_Ptr++;      
    }

    input_ptr += scan_line_incr;
  }

  for (i = 8 - rows; i > 0; i--)
  {
    for (j = 8; j > 0; j--)
    {
      *Y1_Ptr = *(Y1_Ptr - 8);Y1_Ptr++;
      *CB_Ptr = *(CB_Ptr - 8);CB_Ptr++;
      *CR_Ptr = *(CR_Ptr - 8);CR_Ptr++;
    }
  }
}


#define CLIP(color) (unsigned char)(((color)>0xff)?0xff:(((color)<0)?0:(color)))

/* translate RGB24 to YUV444 in input */
void RGB24_2_YCbCr444
(uint8_t * input_ptr, uint8_t * output_ptr, uint32_t image_width,
          uint32_t image_height)
{
  uint32_t i, size;
  uint8_t R, G, B;
  S_INT Y, Cb, Cr;
  size = image_width * image_height;

  for (i = size; i > 0; i--)
  {
    B = *input_ptr++;
    G = *input_ptr++;
    R = *input_ptr++;

    //input_ptr -= 3;
    Y = CLIP((77 * R + 150 * G + 29 * B) >> 8);
    Cb = CLIP(((-43 * R - 85 * G + 128 * B) >> 8) + 128);
    Cr = CLIP(((128 * R - 107 * G - 21 * B) >> 8) + 128);
    *output_ptr++ = (uint8_t) Y;
    *output_ptr++ = (uint8_t) Cb;
    *output_ptr++ = (uint8_t) Cr;
  }
}


/* translate RGB24 to YUV422 in input */
void RGB24_2_YCbCr422
(uint8_t * input_ptr, uint8_t * output_ptr, uint32_t image_width,
          uint32_t image_height)
{
  uint32_t i, size;
  uint8_t R, G, B, R1, G1, B1;
  S_INT Y, Yp, Cb, Cr;
  uint8_t * inbuf = input_ptr;
  size = image_width * image_height;

  for (i = size; i > 0; i--)
  {
    B = inbuf[0];
    G = inbuf[1];
    R = inbuf[2];
    B1 = inbuf[3];
    G1 = inbuf[4];
    R1 = inbuf[5];
    inbuf += 6;
    Y = CLIP((77 * R + 150 * G + 29 * B) >> 8);
    Yp = CLIP((77 * R1 + 150 * G1 + 29 * B1) >> 8);
    Cb = CLIP(((-43 * R - 85 * G + 128 * B) >> 8) + 128);
    Cr = CLIP(((128 * R - 107 * G - 21 * B) >> 8) + 128);
    *output_ptr++ = (uint8_t) Y;
    *output_ptr++ = (uint8_t) Cb;
    *output_ptr++ = (uint8_t) Yp;
    *output_ptr++ = (uint8_t) Cr;
  }
}


/* translate RGB24 to YUV420 in input */
void RGB24_2_YCbCr420(uint8_t * input_ptr, uint8_t * output_ptr, uint32_t image_width,
          uint32_t image_height)
{
  uint32_t i, j;
  uint32_t size;
  uint8_t R, G, B, R1, G1, B1, Rd, Gd, Bd, Rd1, Gd1, Bd1;
  S_INT Y, Yd, Y11, Yd1, Cb, Cr;
  uint8_t * inbuf = input_ptr;
  uint8_t * inbuf1 = input_ptr + (image_width * 3);
  size = image_width * image_height >> 2;

  for (i = size, j = 0; i > 0; i--)
  {
    B = inbuf[0];
    G = inbuf[1];
    R = inbuf[2];
    B1 = inbuf[3];
    G1 = inbuf[4];
    R1 = inbuf[5];
    Bd = inbuf1[0];
    Gd = inbuf1[1];
    Rd = inbuf1[2];
    Bd1 = inbuf1[3];
    Gd1 = inbuf1[4];
    Rd1 = inbuf1[5];
    inbuf += 6;
    inbuf1 += 6;
    j++;

    if (j >= image_width / 2) {
      j = 0;
      inbuf += (image_width * 3);
      inbuf1 += (image_width * 3);
    }

    Y = CLIP((77 * R + 150 * G + 29 * B) >> 8);

    Y11 = CLIP((77 * R1 + 150 * G1 + 29 * B1) >> 8);
    Yd = CLIP((77 * Rd + 150 * Gd + 29 * Bd) >> 8);
    Yd1 = CLIP((77 * Rd1 + 150 * Gd1 + 29 * Bd1) >> 8);
    Cb = CLIP(((-43 * R - 85 * G + 128 * B) >> 8) + 128);
    Cr = CLIP(((128 * R - 107 * G - 21 * B) >> 8) + 128);
		
    *output_ptr++ = (uint8_t) Y;
    *output_ptr++ = (uint8_t) Y11;
    *output_ptr++ = (uint8_t) Yd;
    *output_ptr++ = (uint8_t) Yd1;
    *output_ptr++ = (uint8_t) Cb;
    *output_ptr++ = (uint8_t) Cr;
  }
}


/* translate RGB32 to YUV420 in input */
void RGB32_2_YCbCr420(uint8_t * input_ptr, uint8_t * output_ptr, uint32_t image_width,
            uint32_t image_height)
{
  uint32_t i, j, size;
  uint8_t R, G, B, R1, G1, B1, Rd, Gd, Bd, Rd1, Gd1, Bd1;
  S_INT Y, Yd, Y11, Yd1, Cb, Cr;
  uint8_t * inbuf = input_ptr;
  uint8_t * inbuf1 = input_ptr + (image_width * 4);
  size = image_width * image_height >> 2;

  for (i = size, j = 0; i > 0; i--)

  {
    B = inbuf[0];
    G = inbuf[1];
    R = inbuf[2];
    B1 = inbuf[4];
    G1 = inbuf[5];
    R1 = inbuf[6];
    Bd = inbuf1[0];
    Gd = inbuf1[1];
    Rd = inbuf1[2];
    Bd1 = inbuf1[4];
    Gd1 = inbuf1[5];
    Rd1 = inbuf1[6];
    inbuf += 8;
    inbuf1 += 8;
    j++;

    if (j >= image_width / 2) {
      j = 0;
      inbuf += (image_width * 4);
      inbuf1 += (image_width * 4);
    }

    Y = CLIP((77 * R + 150 * G + 29 * B) >> 8);

    Y11 = CLIP((77 * R1 + 150 * G1 + 29 * B1) >> 8);
    Yd = CLIP((77 * Rd + 150 * Gd + 29 * Bd) >> 8);
    Yd1 = CLIP((77 * Rd1 + 150 * Gd1 + 29 * Bd1) >> 8);
    Cb = CLIP(((-43 * R - 85 * G + 128 * B) >> 8) + 128);
    Cr = CLIP(((128 * R - 107 * G - 21 * B) >> 8) + 128);
    *output_ptr++ = (uint8_t) Y;
    *output_ptr++ = (uint8_t) Y11;
    *output_ptr++ = (uint8_t) Yd;
    *output_ptr++ = (uint8_t) Yd1;
    *output_ptr++ = (uint8_t) Cb;
    *output_ptr++ = (uint8_t) Cr;
  }
}


/* translate RGB565 to YUV420 in input */
void RGB565_2_YCbCr420(uint8_t * input_ptr, uint8_t * output_ptr, uint32_t image_width,
             uint32_t image_height)
{
  uint32_t i, j, size;
  uint8_t R, G, B, R1, G1, B1, Rd, Gd, Bd, Rd1, Gd1, Bd1;
  S_INT Y, Yd, Y11, Yd1, Cb, Cr;
  S_JPEG_RGB16 * inbuf = (S_JPEG_RGB16 *) input_ptr;
  S_JPEG_RGB16 * inbuf1 = inbuf + (image_width);
  size = image_width * image_height >> 2;

  for (i = size, j = 0; i > 0; i--)
  {

    B = inbuf[0].blue << 3;
    G = inbuf[0].green << 2;
    R = inbuf[0].red << 3;
    B1 = inbuf[1].blue << 3;
    G1 = inbuf[1].green << 2;
    R1 = inbuf[1].red << 3;
    Bd = inbuf1[0].blue << 3;
    Gd = inbuf1[0].green << 2;
    Rd = inbuf1[0].red << 3;
    Bd1 = inbuf1[1].blue << 3;
    Gd1 = inbuf[1].green << 2;
    Rd1 = inbuf[1].red << 3;
    inbuf += 2;
    inbuf1 += 2;
    j++;

    if (j >= image_width / 2) {
      j = 0;
      inbuf += (image_width);
      inbuf1 += (image_width);
    }

    Y = CLIP((77 * R + 150 * G + 29 * B) >> 8);

    Y11 = CLIP((77 * R1 + 150 * G1 + 29 * B1) >> 8);
    Yd = CLIP((77 * Rd + 150 * Gd + 29 * Bd) >> 8);
    Yd1 = CLIP((77 * Rd1 + 150 * Gd1 + 29 * Bd1) >> 8);
    Cb = CLIP(((-43 * R - 85 * G + 128 * B) >> 8) + 128);
    Cr = CLIP(((128 * R - 107 * G - 21 * B) >> 8) + 128);
		
    *output_ptr++ = (uint8_t) Y;
    *output_ptr++ = (uint8_t) Y11;
    *output_ptr++ = (uint8_t) Yd;
    *output_ptr++ = (uint8_t) Yd1;
    *output_ptr++ = (uint8_t) Cb;
    *output_ptr++ = (uint8_t) Cr;
  }
}

void RGB24_2_YCbCr400(uint8_t * input_ptr, uint8_t * output_ptr, uint32_t image_width,
          uint32_t image_height)
{
  uint32_t i, size;
  uint8_t R, G, B;
  S_INT Y;
  uint8_t * inbuf = input_ptr;
  size = image_width * image_height;

  for (i = size; i > 0; i--)
  {
    B = inbuf[0];
    G = inbuf[1];
    R = inbuf[2];
    inbuf += 3;
    Y = CLIP((77 * R + 150 * G + 29 * B) >> 8);
    *output_ptr++ = (uint8_t) Y;
  }
}


uint8_t * write_markers(S_JPEG_ENCODER_STRUCTURE *enc,
												uint8_t * output_ptr, uint32_t image_format,
                        uint32_t image_width, uint32_t image_height)
{
  S_UINT i, header_length;
  S_UINT number_of_components;

  // Start of image marker
  *output_ptr++ = 0xFF;
  *output_ptr++ = 0xD8;

  // Quantization table marker
  *output_ptr++ = 0xFF;
  *output_ptr++ = 0xDB;

  // Quantization table length
  *output_ptr++ = 0x00;
  *output_ptr++ = 0x84;

  // Pq, Tq
  *output_ptr++ = 0x00;

  // Lqt table

  for (i = 0; i < 64; i++)
    *output_ptr++ = enc->Lqt[i];

  // Pq, Tq
  *output_ptr++ = 0x01;

  // Cqt table
  for (i = 0; i < 64; i++)
    *output_ptr++ = enc->Cqt[i];

  // huffman table(DHT)
  for (i = 0; i < 210; i++)

  {
    *output_ptr++ = (uint8_t)(markerdata[i] >> 8);
    *output_ptr++ = (uint8_t) markerdata[i];
  }

  if (image_format == FORMAT_CbCr400)
    number_of_components = 1;

  else
    number_of_components = 3;

  // Frame header(SOF)

  // Start of frame marker
  *output_ptr++ = 0xFF;

  *output_ptr++ = 0xC0;

  header_length = (S_UINT)(8 + 3 * number_of_components);

  // Frame header length
  *output_ptr++ = (uint8_t)(header_length >> 8);

  *output_ptr++ = (uint8_t) header_length;

  // Precision (P)
  *output_ptr++ = 0x08;

  // image height
  *output_ptr++ = (uint8_t)(image_height >> 8);

  *output_ptr++ = (uint8_t) image_height;

  // image width
  *output_ptr++ = (uint8_t)(image_width >> 8);

  *output_ptr++ = (uint8_t) image_width;

  // Nf
  *output_ptr++ = number_of_components;

  if (image_format == FORMAT_CbCr400)

  {
    *output_ptr++ = 0x01;
    *output_ptr++ = 0x11;
    *output_ptr++ = 0x00;
  }

  else

  {
    *output_ptr++ = 0x01;

    if (image_format == FORMAT_CbCr420||image_format == FORMAT_CbCr420p)
      *output_ptr++ = 0x22;

    else if (image_format == FORMAT_CbCr422||image_format == FORMAT_CbCr422p)
      *output_ptr++ = 0x21;

    else
      *output_ptr++ = 0x11;

    *output_ptr++ = 0x00;

    *output_ptr++ = 0x02;

    *output_ptr++ = 0x11;

    *output_ptr++ = 0x01;

    *output_ptr++ = 0x03;

    *output_ptr++ = 0x11;

    *output_ptr++ = 0x01;
  }

  // Scan header(SOF)

  // Start of scan marker
  *output_ptr++ = 0xFF;

  *output_ptr++ = 0xDA;

  header_length = (S_UINT)(6 + (number_of_components << 1));

  // Scan header length
  *output_ptr++ = (uint8_t)(header_length >> 8)&0xff;

  *output_ptr++ = (uint8_t) header_length&0xff;

  // Ns
  *output_ptr++ = number_of_components;

  if (image_format == FORMAT_CbCr400)

  {
    *output_ptr++ = 0x01;
    *output_ptr++ = 0x00;
  }

  else

  {
    *output_ptr++ = 0x01;
    *output_ptr++ = 0x00;
    *output_ptr++ = 0x02;
    *output_ptr++ = 0x11;
    *output_ptr++ = 0x03;
    *output_ptr++ = 0x11;
  }

  *output_ptr++ = 0x00;

  *output_ptr++ = 0x3F;
  *output_ptr++ = 0x00;
  return output_ptr;
}

S_PIXEL * huffman(S_JPEG_ENCODER_STRUCTURE * enc,
                  COMPONENT component, S_PIXEL * output_ptr)
{
  S_UINT i;
  S_UINT * DcCodeTable, *DcSizeTable, *AcCodeTable, *AcSizeTable;
  S_INT * Temp_Ptr, Coeff, LastDc;
  S_UINT AbsCoeff, HuffCode, HuffSize;
  S_UINT RunLength = 0, DataSize = 0;
  S_UINT index;
  S_INT  bits_in_next_word;
  S_UINT numbits;
  
  uint32_t data;
  Temp_Ptr = enc->Temp; //?
	
  Coeff = *Temp_Ptr++;

  if (component == COMPONENT_Y)
  {
    DcCodeTable = luminance_dc_code_table;
    DcSizeTable = luminance_dc_size_table;
    AcCodeTable = luminance_ac_code_table;
    AcSizeTable = luminance_ac_size_table;
    
    LastDc = enc->ldc1;
    enc->ldc1 = Coeff;
		
  } else {
    DcCodeTable = chrominance_dc_code_table;
    DcSizeTable = chrominance_dc_size_table;
    AcCodeTable = chrominance_ac_code_table;
    AcSizeTable = chrominance_ac_size_table;

    if (component == COMPONENT_CB)
    {
      LastDc = enc->ldc2;
      enc->ldc2 = Coeff;
    } else { //COMPONENT_CR
      LastDc = enc->ldc3;
      enc->ldc3 = Coeff;
    }
  }

  Coeff -= LastDc;

  AbsCoeff = (Coeff < 0) ? -Coeff-- : Coeff;//?

  while (AbsCoeff != 0)
  {
    AbsCoeff >>= 1;
    DataSize++;
  }

  HuffCode = DcCodeTable[DataSize];

  HuffSize = DcSizeTable[DataSize];
  Coeff &= (1 << DataSize) - 1;
  data = (HuffCode << DataSize) | Coeff;
  numbits = HuffSize + DataSize;
  
  //PUTBITS
  {
    bits_in_next_word = (S_INT)(enc->bitindex + numbits - 32);

    if (bits_in_next_word < 0) {
      enc->lcode = (enc->lcode << numbits) | data;
      enc->bitindex += numbits;
    } else {
      enc->lcode = (enc->lcode << (32 - enc->bitindex)) | (data >> bits_in_next_word);

      if ((*output_ptr++ = (S_PIXEL)(enc->lcode >> 24)) == 0xff)
        *output_ptr++ = 0;

      if ((*output_ptr++ = (S_PIXEL)(enc->lcode >> 16)) == 0xff)
        *output_ptr++ = 0;

      if ((*output_ptr++ = (S_PIXEL)(enc->lcode >> 8)) == 0xff)
        *output_ptr++ = 0;

      if ((*output_ptr++ = (S_PIXEL) enc->lcode) == 0xff)
        *output_ptr++ = 0;

      enc->lcode = data;

      enc->bitindex = bits_in_next_word;
    }
  }

  for (i = S_BLOCK_SIZE-1; i > 0; i--)
  {
    if ((Coeff = *Temp_Ptr++) != 0)
    {
      while (RunLength > 15)
      {
        RunLength -= 16;
        data = AcCodeTable[161];
        numbits = AcSizeTable[161];
        //PUTBITS
        {
          bits_in_next_word = (S_INT)(enc->bitindex + numbits - 32);

          if (bits_in_next_word < 0) {
            enc->lcode = (enc->lcode << numbits) | data;
            enc->bitindex += numbits;
          } else {
            enc->lcode = (enc->lcode << (32 - enc->bitindex)) | (data >> bits_in_next_word);

            if ((*output_ptr++ = (uint8_t)(enc->lcode >> 24)) == 0xff)
              *output_ptr++ = 0;

            if ((*output_ptr++ = (uint8_t)(enc->lcode >> 16)) == 0xff)
              *output_ptr++ = 0;

            if ((*output_ptr++ = (uint8_t)(enc->lcode >> 8)) == 0xff)
              *output_ptr++ = 0;

            if ((*output_ptr++ = (uint8_t) enc->lcode) == 0xff)
              *output_ptr++ = 0;

            enc->lcode = data;

            enc->bitindex = bits_in_next_word;
          }
        }
      }

      AbsCoeff = (Coeff < 0) ? -Coeff-- : Coeff;

      if (AbsCoeff >> 8 == 0)
        DataSize = bitsize[AbsCoeff];

      else
        DataSize = bitsize[AbsCoeff >> 8] + 8;

      index = RunLength * 10 + DataSize;

      HuffCode = AcCodeTable[index];

      HuffSize = AcSizeTable[index];

      Coeff &= (1 << DataSize) - 1;

      data = (HuffCode << DataSize) | Coeff;

      numbits = HuffSize + DataSize;

      // PUTBITS
      {
        bits_in_next_word = (S_INT)(enc->bitindex + numbits - 32);

        if (bits_in_next_word < 0) {
          enc->lcode = (enc->lcode << numbits) | data;
          enc->bitindex += numbits;
        } else {
          enc->lcode = (enc->lcode << (32 - enc->bitindex)) | (data >> bits_in_next_word);

          if ((*output_ptr++ = (uint8_t)(enc->lcode >> 24)) == 0xff)
            *output_ptr++ = 0;

          if ((*output_ptr++ = (uint8_t)(enc->lcode >> 16)) == 0xff)
            *output_ptr++ = 0;

          if ((*output_ptr++ = (uint8_t)(enc->lcode >> 8)) == 0xff)
            *output_ptr++ = 0;

          if ((*output_ptr++ = (uint8_t) enc->lcode) == 0xff)
            *output_ptr++ = 0;

          enc->lcode = data;

          enc->bitindex = bits_in_next_word;
        }
      }

      RunLength = 0;
    }

    else
      RunLength++;
  }

  if (RunLength != 0)

  {
    data = AcCodeTable[0];
    numbits = AcSizeTable[0];
    // PUTBITS
    {
      bits_in_next_word = (S_INT)(enc->bitindex + numbits - 32);

      if (bits_in_next_word < 0) {
        enc->lcode = (enc->lcode << numbits) | data;
        enc->bitindex += numbits;
      } else {
        enc->lcode = (enc->lcode << (32 - enc->bitindex)) | (data >> bits_in_next_word);

        if ((*output_ptr++ = (uint8_t)(enc->lcode >> 24)) == 0xff)
          *output_ptr++ = 0;

        if ((*output_ptr++ = (uint8_t)(enc->lcode >> 16)) == 0xff)
          *output_ptr++ = 0;

        if ((*output_ptr++ = (uint8_t)(enc->lcode >> 8)) == 0xff)
          *output_ptr++ = 0;

        if ((*output_ptr++ = (uint8_t) enc->lcode) == 0xff)
          *output_ptr++ = 0;

        enc->lcode = data;

        enc->bitindex = bits_in_next_word;
      }
    }
  }

  return output_ptr;
}


/* For bit Stuffing and EOI marker */
uint8_t * close_bitstream(S_JPEG_ENCODER_STRUCTURE *enc,uint8_t * output_ptr)
{
  S_UINT i, count;
  uint8_t * ptr;

  if (enc->bitindex > 0)
  {
    enc->lcode <<= (32 - enc->bitindex);
    count = (enc->bitindex + 7) >> 3;
    ptr = (uint8_t *) & enc->lcode + 3;

    for (i = count; i > 0; i--)

    {
      if ((*output_ptr++ = *ptr--) == 0xff)
        *output_ptr++ = 0;
    }
  }

  // End of image marker
  *output_ptr++ = 0xFF;

  *output_ptr++ = 0xD9;

  return output_ptr;
}


