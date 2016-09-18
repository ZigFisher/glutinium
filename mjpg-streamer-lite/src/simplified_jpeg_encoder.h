/***************************************************************************#
# simplified_jpeg_encoder: library to encode a jpeg frame                   #
# from various input palette.																						    #
# simplified_jpeg_encoder works for embedded device without libjpeg         #
#.                                                                          #
#     Copyright (C) 2010  Vladimir S. Fonov                                 #
# Based on jpegenc code from Michel Xhaard   Copyright (C) 2005             #
#                                                                           #
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
#ifndef __SIMPLIFIED_JPEG_ENCODER_H__
#define  __SIMPLIFIED_JPEG_ENCODER_H__

#include <stdint.h>

//  change the type depending on what's faster on given architecture
typedef uint32_t S_UINT;
typedef int32_t  S_INT;
typedef uint8_t  S_PIXEL;

#define S_BLOCK_SIZE  64
#define INLINE  inline  //inline function definition


typedef struct S_JPEG_ENCODER_STRUCTURE_tag
{
  //internal state
  S_UINT mcu_width;
  S_UINT mcu_height;
	
  S_UINT horizontal_mcus;
  S_UINT vertical_mcus;
	
  S_UINT rows_in_bottom_mcus;
  S_UINT cols_in_right_mcus;
  S_UINT length_minus_mcu_width;
  S_UINT length_minus_width;
	
  S_UINT mcu_width_size;
	S_UINT mcu_height_size;
  S_UINT mcu_line_offset;
	
  S_INT ldc1;
  S_INT ldc2;
  S_INT ldc3;
  S_UINT rows;
  S_UINT cols;
  uint32_t scan_line_incr;
	
	//offset for Cr and Cb component in 422p and 420p formats
  uint32_t CrOffset;
  uint32_t CbOffset;

	//quantization tables
  S_PIXEL Lqt[S_BLOCK_SIZE];
  S_PIXEL Cqt[S_BLOCK_SIZE];
  S_UINT ILqt[S_BLOCK_SIZE];
  S_UINT ICqt[S_BLOCK_SIZE];
  //temporary storage
  S_INT    Y1[S_BLOCK_SIZE];
  S_INT    Y2[S_BLOCK_SIZE];
  S_INT    Y3[S_BLOCK_SIZE];
  S_INT    Y4[S_BLOCK_SIZE];
  S_INT    CB[S_BLOCK_SIZE];
  S_INT    CR[S_BLOCK_SIZE];
  
	 //Huffman coded internal state
  S_INT  Temp[S_BLOCK_SIZE];
  int32_t lcode;
  S_INT bitindex;
	
	void (*read_format)(struct S_JPEG_ENCODER_STRUCTURE_tag * enc, uint8_t * input_ptr,S_UINT row,S_UINT col);
} S_JPEG_ENCODER_STRUCTURE;

/********* Image_format know by the encoder *********************/ 
// WARNING: FORMAT_CbCr420p and FORMAT_CbCr444p are not properly supported yet
enum { FORMAT_CbCr400=0,FORMAT_CbCr420,FORMAT_CbCr422,FORMAT_CbCr444,FORMAT_CbCr420p,FORMAT_CbCr422p,FORMAT_CbCr444p};

/* encode picture input to output 
quality factor 
image_format look the define 
image_width image_height of the input picture
return the encoded size in Byte
*/ 
uint32_t s_encode_image(uint8_t * input_ptr, uint8_t * output_ptr,
                        uint32_t quality_factor, int image_format,
                        uint32_t image_width, uint32_t image_height,
                        uint32_t output_buffer_size);
												
void RGB24_2_YCbCr444(uint8_t * input_ptr, uint8_t * output_ptr,
                      uint32_t image_width, uint32_t image_height);
											
void RGB24_2_YCbCr422(uint8_t * input_ptr, uint8_t * output_ptr,
                      uint32_t image_width, uint32_t image_height);
											
void RGB24_2_YCbCr420(uint8_t * input_ptr, uint8_t * output_ptr,
                      uint32_t image_width, uint32_t image_height);
											
void RGB24_2_YCbCr400(uint8_t * input_ptr, uint8_t * output_ptr,
                      uint32_t image_width, uint32_t image_height);
											
void RGB565_2_YCbCr420(uint8_t * input_ptr, uint8_t * output_ptr,
                         uint32_t image_width, uint32_t image_height);
												 
void RGB32_2_YCbCr420(uint8_t * input_ptr, uint8_t * output_ptr,
                        uint32_t image_width, uint32_t image_height);
												
#endif // __SIMPLIFIED_JPEG_ENCODER_H__