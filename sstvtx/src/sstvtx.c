/*
 * sstvtx - Martin 1 mode SSTV generator
 *
 * $Id: sstvtx.c 82 2008-07-30 12:13:59Z dj1yfk $
 *
 * Copyright (c) 2008 Fabian Kurz, DJ1YFK
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

/*
 * Compile with: gcc sstvtx.c -lsndfile -ljpeg -lm -o sstvtx
 * apt-get install libjpeg-dev libsndfile1-dev
 *
 */



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sndfile.h>
#include "jpeglib.h"
#include "jerror.h"
#include <setjmp.h>
#include <unistd.h>

#define PIXEL 1
#define SYNC 0

/* JPEG input image will be read into this array; picture size 320x256,
 * RGB components in separate rows */
static unsigned char pic[320][3*256];	

typedef struct {
	int samplerate;
	int sync_len;		/* length of synch impulse in samples */
	int pix_len;		/* length of normal pixel in samples */
	int x_width;
	double phase;
	char *infile;
	char *outfile;
} SSTV;

void die(char *err);
int read_JPEG_file (char *filename);
int tx_sync(SNDFILE *snd, SSTV *sstv);
int tx_pixel(unsigned char p, SNDFILE *snd, SSTV *sstv, int typ);
int init_sstv(SSTV *sstv);
void help (void);

int main (int argc, char **argv) {
	int x,y;

	SSTV sstv;
	SNDFILE *out;
	SF_INFO out_info;

	sstv.samplerate = 48000;
	sstv.x_width = 320;
	sstv.phase = 0;
	sstv.outfile = "out.wav";
	sstv.infile = "input.jpg";

	while ((x = getopt(argc, argv, "s:i:o:x:h")) != -1) {
		switch (x) {
			case 's':
				sstv.samplerate = atoi(optarg);
				break;
			case 'i':
				sstv.infile = optarg;
				break;
			case 'o':
				sstv.outfile = optarg;
				break;
			case 'h':
				help();
				break;
			case 'x':
				sstv.x_width= atoi(optarg);
				break;
		}
	}


	out_info.samplerate = sstv.samplerate;
	out_info.channels = 1;
	out_info.format = SF_FORMAT_WAV|SF_FORMAT_PCM_16;
	
	if ((out = sf_open(sstv.outfile, SFM_WRITE, &out_info)) == NULL) {
		die("Opening soundfile failed.");
	}

	read_JPEG_file(sstv.infile);

	init_sstv(&sstv);

	for (y=0; y < 3*256; y++) {
		for (x = 0; x < sstv.x_width; x++) {
			tx_pixel(pic[x][y], out, &sstv, PIXEL);
		}
		if (!(y % 3)) {
			tx_pixel(0, out, &sstv, SYNC);
		}
	}

	sf_close(out);

return 0;
}

void die(char *err) {
	fprintf(stderr, "Error: %s\n", err);
	exit(EXIT_FAILURE);
}

int read_JPEG_file (char *filename) {
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE *infile;
	JSAMPARRAY buffer;
	int row_stride;
	int i, row=0;

	if ((infile = fopen(filename, "rb")) == NULL) {
		die("Can't open input file.");
	}

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, infile);
	(void) jpeg_read_header(&cinfo, TRUE);
	(void) jpeg_start_decompress(&cinfo);

	if (cinfo.output_width != 320 || cinfo.output_height != 256) {
		die ("Input file format wrong. Need 320x256.");
	}

	row_stride = cinfo.output_width * cinfo.output_components;

	buffer = (*cinfo.mem->alloc_sarray)
			((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	while (cinfo.output_scanline < cinfo.output_height) {
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);

		/* each line in buffer: R,G,B,R,G,B -> put to array RRR/GGG/BBB */
		for (i=0; i < cinfo.output_width; i++) {
			pic[i][row] = buffer[0][3*i];
			pic[i][row+1] = buffer[0][3*i+1];
			pic[i][row+2] = buffer[0][3*i+2];
		}
		row+=3;
	}

	(void) jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	fclose(infile);

	return 0;
}

int init_sstv(SSTV *sstv) {
	/* sync len = 5ms */
	sstv->sync_len = sstv->samplerate * 0.005;

	/* pixel length, in samples, total pixels 245760, 114s - 1.28s sync 
	 * results in 0.45865885 ms / pixel */

	sstv->pix_len = (int) sstv->samplerate * 0.00045865885416666666;

	return 0;
}


int tx_pixel(unsigned char p, SNDFILE *snd, SSTV *sstv, int typ) {
	int freq;
	short int buf[500];
	int j, len, i=0;
	double phase=0;

	/* normalize char to frequency range of 1500-2300Hz */
	if (typ == PIXEL) {
		freq = (int) 1500 + 800.0*p/255.0;
		len = sstv->pix_len;
	}
	else {
		freq = 1200;
		len = sstv->sync_len;
	}

	/* find back to old phase */
	while (phase < sstv->phase) {
		i++;
		phase = 2.0*M_PI*freq*i/sstv->samplerate;
	}

	/* make pixel, starting with that phase */
	for (j=0; j < len; j++) {
		phase = 2.0*M_PI*freq*i/sstv->samplerate;
		i++;
		buf[j] = (short int) 25000 * sin(phase);
	}

	if (phase > 2*M_PI) {
		phase -= 2*M_PI;
	}

	sstv->phase = phase;

	if ((i = sf_write_short(snd, buf, j)) != j) {
		die("Writing Pixel failed.");
	}
	return 0;
}


void help (void) {
	printf("sstvtx - generates a SSTV Martin 1 image\n");
	printf("\n");
	printf("Usage: sstvtx -i input.jpg -o output.wav -x x_width -s samplerate\n");
	printf("\n");
	printf("Written by Fabian Kurz, DJ1YFK; http://fkurz.net/ham/stuff.html#sstvtx\n\n");
	exit(EXIT_SUCCESS);
}


