// This gets video from the C328 camera, saves it, & shows it in a preview window.

// Adapted from http://gizmologi.st/2009/04/taking-pictures-with-arduino/

// XBee configuration:
// enter command mode
// +++
// set the frequency
// atch17

// set destination address on module 1
// atdla7c9
// set receive address on module 1
// atmye8a6

// set destination address on module 2
// atdle8a6
// set receive address on module 2
// atmya7c9

// packet timeout
// atro03
// baud rate  07=115200 06=57600
// atbd07
// change terminal baud rate now

// flash configuration
// atwr
// exit command mode
// atcn

// make video;./video






#include <errno.h>
#include <fcntl.h>
#include <linux/serial.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>




// Most configurable
#define SERIAL_PATH "/dev/ttyUSB0"
#define JPEG_PREFIX "/www/"
#define RESOLUTION JR_640x480
//#define RESOLUTION JR_320x240
//#define RESOLUTION JR_160x128
#define WINDOW_X 850
#define WINDOW_Y 0
#define WINDOW_W 400
#define WINDOW_H 300
// Flip vertically
#define FLIP_VIDEO



// Use microcontroller to send ACKs
//#define USE_MICRO

#undef USE_X11

#ifdef USE_X11
#include <sys/shm.h>
#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include <X11/Xutil.h>
#include "jpeglib.h"
#include <setjmp.h>

Display *display;
int screen;
Window rootwin;
Visual *vis;
int default_depth;
Window win;
XImage *x_image;
XShmSegmentInfo shm_info;
GC gc;
unsigned char *x_bitmap;
int x_bytes_per_line;
unsigned char **jpeg_rows = 0;
unsigned char *jpeg_bitmap = 0;
struct jpeg_decompress_struct cinfo;
struct my_jpeg_error_mgr jerr;

#endif



// Less configurable
// timeout in us
#define SERIAL_TIMEOUT 1000000
#define SERIAL_BAUD B115200
// Limited by XBee buffer
#define PACKAGE_SIZE 128

// Color types
#define CT_GRAYSCALE_2 0x01
#define CT_GRAYSCALE_4 0x02
#define CT_GRAYSCALE_8 0x03
#define CT_COLOR_12    0x05
#define CT_COLOR_16    0x06
#define CT_JPEG        0x07

// preview resolution
#define PR_80x60       0x01
#define PR_160x120     0x03

// snapshot resolution
#define JR_80x64       0x01
#define JR_160x128     0x03
#define JR_320x240     0x05
#define JR_640x480     0x07

// snapshot type
#define ST_COMPRESSED   0x00
#define ST_UNCOMPRESSED 0x01

// frequency type
#define FT_50HZ 	   0x00
#define FT_60HZ 	   0x01

// picture type
#define PT_SNAPSHOT    0x01
#define PT_PREVIEW     0x02
#define PT_JPEG        0x05



#define CMD_PREFIX 0xAA
#define CMD_SYNC 0x0D
#define CMD_ACK 0x0E
#define CMD_NAK 0x0F
#define CMD_INIT 0x01
#define CMD_DATA 0x0A
#define CMD_RESET 0x08
#define CMD_POWEROFF 0x09
#define CMD_BAUDRATE 0x07
#define CMD_PACKAGESIZE 0x06
#define CMD_SNAPSHOT 0x05
#define CMD_GETPICTURE 0x04
#define CMD_LIGHTFREQ 0x13


#define LAST_JPEG_ACK 0xF0F0
#define RAW_ACK 0x0A

// The byte size of commands
#define CMD_SIZE 6

// Number of sync attempts to try before giving up
#define MAX_SYNC_ATTEMPTS 50

// How long to wait for serial communication responses in us
#define RESPONSE_DELAY 100000

// How long to wait getween snapshot & getpicture commands
#define PROCESS_DELAY 200000

// How long to wait between data packages in us
#define PACKAGE_DELAY 1000000

// The byte offset where image data starts in a package
// data package
#define PACKAGE_DATA_START 4

// The byte offset from the end of a data package where
// JPEG image data ends
#define PACKAGE_DATA_END_OFFSET 2

// Maximum allowed errors when reading picture data
#define MAX_ERRORS 5

#define MAX_PICTURE_SIZE 0x100000


unsigned char command[CMD_SIZE];
unsigned char receive_cmd[CMD_SIZE];
int serial_fd =  -1;
int picture_number = 0;

// Returns the FD of the serial port
int init_serial(char *path, int baud, int custom_baud)
{
	struct termios term;

// Initialize serial port
	int fd = open(path, O_RDWR | O_NOCTTY | O_SYNC);
	if(fd < 0)
	{
		printf("init_serial %d: path=%s: %s\n", __LINE__, path, strerror(errno));
		return -1;
	}
	
	if (tcgetattr(fd, &term))
	{
		printf("init_serial %d: path=%s %s\n", __LINE__, path, strerror(errno));
		close(fd);
		return -1;
	}



/*
 * printf("init_serial: %d path=%s iflag=0x%08x oflag=0x%08x cflag=0x%08x\n", 
 * __LINE__, 
 * path, 
 * term.c_iflag, 
 * term.c_oflag, 
 * term.c_cflag);
 */
	tcflush(fd, TCIOFLUSH);
	cfsetispeed(&term, baud);
	cfsetospeed(&term, baud);
//	term.c_iflag = IGNBRK;
	term.c_iflag = 0;
	term.c_oflag = 0;
	term.c_lflag = 0;
//	term.c_cflag &= ~(PARENB | PARODD | CRTSCTS | CSTOPB | CSIZE);
//	term.c_cflag |= CS8;
	term.c_cc[VTIME] = 1;
	term.c_cc[VMIN] = 1;
/*
 * printf("init_serial: %d path=%s iflag=0x%08x oflag=0x%08x cflag=0x%08x\n", 
 * __LINE__, 
 * path, 
 * term.c_iflag, 
 * term.c_oflag, 
 * term.c_cflag);
 */
	if(tcsetattr(fd, TCSANOW, &term))
	{
		printf("init_serial %d: path=%s %s\n", __LINE__, path, strerror(errno));
		close(fd);
		return -1;
	}

	return fd;
}

// Send a character
void write_char(int fd, unsigned char c)
{
	int result;
	do
	{
		result = write(fd, &c, 1);
	} while(!result);
}

void write_buffer(int fd, unsigned char *data, int size)
{
	int result = 0;
	int bytes_sent = 0;
	do
	{
		bytes_sent = write(fd, data + result, size - result);
		if(bytes_sent > 0) result += bytes_sent;
	}while(result < size);
}

// Read a character
unsigned char read_char(int fd)
{
	unsigned char c;
	int result;
	do
	{
		result = read(fd, &c, 1);
	} while(result <= 0);
	return c;
}

// Read a character with a timeout in microseconds
unsigned char read_char_timeout(int fd, int timeout, int *failed)
{
	unsigned char c = 0xff;
	fd_set rfds;
	struct timeval timeout_struct;
	timeout_struct.tv_sec = 0;
	timeout_struct.tv_usec = timeout;
	*failed = 0;

	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);
	int result = select(fd + 1, &rfds, 0, 0, &timeout_struct);
	if(result > 0)
	{
		c = read_char(fd);
	}
	else
		*failed = 1;

	return c;
}












static void create_command(unsigned char cmd, 
	unsigned char param1, 
	unsigned char param2, 
	unsigned char param3, 
	unsigned char param4 )
{
	command[0] = CMD_PREFIX;
	command[1] = cmd;
	command[2] = param1;
	command[3] = param2;
	command[4] = param3;
	command[5] = param4;
}

static void send_command()
{
	const int debug = 0;
	if(debug) printf("send_command %d %0x02xd %0x02xd %0x02xd %0x02xd %0x02xd %0x02x\n",
		__LINE__,
		command[0],
		command[1],
		command[2],
		command[3],
		command[4],
		command[5]);
	write_buffer(serial_fd, command, CMD_SIZE);
}

static int wait_for_response(int timeout, 
	unsigned char *buffer, 
	int buffer_size)
{
	int count = 0;
	int error = 0;
	const int debug = 0;

	bzero(buffer, buffer_size);
	while(count < buffer_size && !error)
	{
		if(debug) printf("wait_for_response %d\n", __LINE__);
		buffer[count] = read_char_timeout(
			serial_fd, 
			timeout, 
			&error);
		if(debug) printf("wait_for_response %d 0x%02x error=%d\n", 
			__LINE__, 
			buffer[count],
			error);
		count++;
	}
	return !error;
}

static int wait_for_ack(int timeout, int cmd_id)
{
	int result = wait_for_response(timeout, receive_cmd, CMD_SIZE);
	if(result && 
		receive_cmd[1] == CMD_ACK &&
		receive_cmd[2] == cmd_id)
	{
		return 1;
	}

	return 0;
}

static void send_ack(int command, int package_id)
{
	create_command(CMD_ACK, 
		command, 
		0, 
		(package_id & 0xff), 
		(package_id >> 8));
	send_command();
}

static int camera_sync()
{
	const int debug = 1;



	create_command(CMD_SYNC, 0, 0, 0, 0);

	while(1)
	{

// Flush serial port
		while(1)
		{
			int error = 0;
			int c = read_char_timeout(
				serial_fd, 
				RESPONSE_DELAY, 
				&error);
			printf("camera_sync %d c=0x%02x\n", __LINE__, c);
			if(error) break;
		}

		int attempts = 0;
		printf("camera_sync %d\n", __LINE__);
		while(attempts < MAX_SYNC_ATTEMPTS)
		{
			send_command();
			int result = wait_for_ack(RESPONSE_DELAY, CMD_SYNC);

			if(debug) printf("camera_sync %d result=%d receive_cmd=0x%x\n", 
				__LINE__,
				result,
				receive_cmd[1]);
			if(result)
			{
				result = wait_for_response(RESPONSE_DELAY, receive_cmd, CMD_SIZE);

				if(debug) printf("camera_sync %d result=%d receive_cmd=0x%x\n", 
					__LINE__,
					result,
					receive_cmd[1]);
				if(result && receive_cmd[1] == CMD_SYNC)
				{
					if(debug) printf("camera_sync %d\n", __LINE__);
					create_command(CMD_ACK, CMD_SYNC, 0, 0, 0);
					send_command();
					return 1;
				}
			}
			else
			{
				printf("camera_sync %d attempts=%d\n", __LINE__, attempts);
			}

			attempts++;
		}

		if(attempts >= MAX_SYNC_ATTEMPTS)
		{
			printf("camera_sync %d 10 second sleep\n", __LINE__);
			sleep(10);
		}
	}

	return 0;
}




static int camera_init(int color_type, 
	int preview_res, 
	int snapshot_res)
{
	create_command(CMD_INIT, 0, color_type, preview_res, snapshot_res);
	send_command();
	return wait_for_ack(RESPONSE_DELAY, CMD_INIT);
}

static int camera_package_size(int bytes)
{
	create_command(CMD_PACKAGESIZE, 0x08, bytes & 0xff, (bytes >> 8), 0);
	send_command();

	int result = wait_for_ack(RESPONSE_DELAY, CMD_PACKAGESIZE);
	printf("camera_package_size %d result=%d\n", __LINE__, result);
	return result;
}

static int camera_light_freq(int freq)
{
  	create_command( CMD_LIGHTFREQ, freq, 0, 0, 0 );
  	send_command();

  	int result = wait_for_ack( RESPONSE_DELAY, CMD_LIGHTFREQ );
	printf("camera_light_freq %d result=%d\n", __LINE__, result);
	return result;
}

// skip - number of frames to skip before snapshot
static int camera_snapshot(int type, int skip)
{
	create_command( CMD_SNAPSHOT, type, skip & 0xff, (skip >> 8), 0 ); 
  	send_command();

  	int result = wait_for_ack( RESPONSE_DELAY, CMD_SNAPSHOT );
	printf("camera_snapshot %d result=%d\n", __LINE__, result);
	return result;
}




#ifdef USE_X11


typedef struct 
{
	struct jpeg_source_mgr pub;	/* public fields */

	JOCTET * buffer;		/* start of buffer */
	int bytes;             /* total size of buffer */
} jpeg_source_mgr;
typedef jpeg_source_mgr* jpeg_src_ptr;


struct my_jpeg_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */
  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_jpeg_error_mgr* my_jpeg_error_ptr;
struct my_jpeg_error_mgr my_jpeg_error;

METHODDEF(void) init_source(j_decompress_ptr cinfo)
{
    jpeg_src_ptr src = (jpeg_src_ptr) cinfo->src;
}

METHODDEF(boolean) fill_input_buffer(j_decompress_ptr cinfo)
{
	jpeg_src_ptr src = (jpeg_src_ptr) cinfo->src;
#define   M_EOI     0xd9

	src->buffer[0] = (JOCTET)0xFF;
	src->buffer[1] = (JOCTET)M_EOI;
	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = 2;

	return TRUE;
}


METHODDEF(void) skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
	jpeg_src_ptr src = (jpeg_src_ptr)cinfo->src;

	src->pub.next_input_byte += (size_t)num_bytes;
	src->pub.bytes_in_buffer -= (size_t)num_bytes;
}


METHODDEF(void) term_source(j_decompress_ptr cinfo)
{
}


METHODDEF(void) my_jpeg_error_exit (j_common_ptr cinfo)
{
/* cinfo->err really points to a mjpeg_error_mgr struct, so coerce pointer */
  	my_jpeg_error_ptr mjpegerr = (my_jpeg_error_ptr) cinfo->err;

/* Always display the message. */
/* We could postpone this until after returning, if we chose. */
  	(*cinfo->err->output_message) (cinfo);

/* Return control to the setjmp point */
  	longjmp(mjpegerr->setjmp_buffer, 1);
}




static void init_window()
{
	display = XOpenDisplay(0);
	screen = DefaultScreen(display);
	rootwin = RootWindow(display, screen);
	vis = DefaultVisual(display, screen);
	default_depth = DefaultDepth(display, screen);

	unsigned long mask;
	XSetWindowAttributes attr;
	XSizeHints size_hints;
	
	mask = 0;
	win = XCreateWindow(display, 
		rootwin, 
		WINDOW_X, 
		WINDOW_Y, 
		WINDOW_W, 
		WINDOW_H, 
		0, 
		default_depth, 
		InputOutput, 
		vis, 
		mask, 
		&attr);


	unsigned long gcmask;
	gcmask = 0;
	XGCValues gcvalues;
	gc = XCreateGC(display, rootwin, gcmask, &gcvalues);

	size_hints.flags = PSize | PMinSize | PMaxSize | PPosition;
	size_hints.width = WINDOW_W;
	size_hints.height = WINDOW_H;
	size_hints.min_width = WINDOW_W;
	size_hints.max_width = WINDOW_W; 
	size_hints.min_height = WINDOW_H;
	size_hints.max_height = WINDOW_H; 
	size_hints.x = WINDOW_X;
	size_hints.y = WINDOW_Y;
	XSetStandardProperties(display, 
		win, 
		"VicaCopter: Video", 
		"VicaCopter: Video", 
		None, 
		0, 
		0, 
		&size_hints);
	XMapWindow(display, win); 


	
// Create bitmap
	bzero(&shm_info, sizeof(XShmSegmentInfo));
	x_image = XShmCreateImage(display, 
		vis, 
		default_depth, 
		ZPixmap, 
		(char*)NULL, 
		&shm_info, 
		WINDOW_W, 
		WINDOW_H);
	x_bytes_per_line = x_image->bytes_per_line;
	shm_info.shmid = shmget(IPC_PRIVATE, 
		WINDOW_H * x_bytes_per_line + 4, 
		IPC_CREAT | 0777);
	x_bitmap = (unsigned char *)shmat(shm_info.shmid, NULL, 0);
// This causes it to automatically delete when the program exits.
	shmctl(shm_info.shmid, IPC_RMID, 0);
	x_image->data = shm_info.shmaddr = (char*)x_bitmap;  // setting ximage->data stops BadValue
	shm_info.readOnly = 0;
	XShmAttach(display, &shm_info);
	XFlush(display);


// Initialize JPEG decompressor
	cinfo.err = jpeg_std_error(&(my_jpeg_error.pub));
	my_jpeg_error.pub.error_exit = my_jpeg_error_exit;
	jpeg_create_decompress(&cinfo);
// Create buffer source
	cinfo.src = (struct jpeg_source_mgr*)
    	(*cinfo.mem->alloc_small)((j_common_ptr)&cinfo, 
        JPOOL_PERMANENT,
		sizeof(jpeg_source_mgr));
	jpeg_src_ptr src = (jpeg_src_ptr)cinfo.src;
	src->pub.init_source = init_source;
	src->pub.fill_input_buffer = fill_input_buffer;
	src->pub.skip_input_data = skip_input_data;
	src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
	src->pub.term_source = term_source;
}




static void draw_picture(unsigned char *picture_data, int picture_size)
{
	int i, j;

	if(setjmp(my_jpeg_error.setjmp_buffer))
	{
/* If we get here, the JPEG code has signaled an error. */
		return;
	}


	jpeg_src_ptr src = (jpeg_src_ptr)cinfo.src;
	src->pub.bytes_in_buffer = picture_size;
	src->pub.next_input_byte = picture_data;
	src->buffer = picture_data;
	src->bytes = picture_size;
	
	
	jpeg_read_header(&cinfo, 1);
	jpeg_start_decompress(&cinfo);
	
	int jpeg_w = cinfo.output_width;
	int jpeg_h = cinfo.output_height;
	if(jpeg_w > 640) jpeg_w = 640;
	if(jpeg_h > 480) jpeg_h = 480;
	
	if(!jpeg_rows)
	{
		jpeg_rows = calloc(1, sizeof(unsigned char*) * jpeg_h);
		jpeg_bitmap = calloc(1, 3 * jpeg_w * jpeg_h);
		for(i = 0; i < jpeg_h; i++)
			jpeg_rows[i] = jpeg_bitmap + i * jpeg_w * 3;
	}

	while(cinfo.output_scanline < jpeg_h)
	{
		int num_scanlines = jpeg_read_scanlines(&cinfo, 
			&jpeg_rows[cinfo.output_scanline],
			jpeg_h - cinfo.output_scanline);
	}
	jpeg_finish_decompress(&cinfo);


// Copy to X bitmap
	for(i = 0; i < WINDOW_H; i++)
	{
		unsigned char *output_row = x_bitmap + i * x_bytes_per_line;
#ifdef FLIP_VIDEO
		int in_y = (WINDOW_H - 1 - i) * jpeg_h / WINDOW_H;
#else
		int in_y = i * jpeg_h / WINDOW_H;
#endif

		if(in_y < 0) in_y = 0;
		if(in_y >= jpeg_h) in_y = jpeg_h - 1;
		unsigned char *input_row = jpeg_rows[in_y];

		for(j = 0; j < WINDOW_W; j++)
		{
			int in_x = j * jpeg_w / WINDOW_W;
			if(in_x < 0) in_x = 0;
			if(in_x >= jpeg_w) in_x = jpeg_w;
// BGRX
			*output_row++ = input_row[in_x * 3 + 2];
			*output_row++ = input_row[in_x * 3 + 1];
			*output_row++ = input_row[in_x * 3 + 0];
			*output_row++ = 0;
		}
	}


// Blit X bitmap
	XShmPutImage(display, 
		win, 
		gc, 
		x_image, 
		0, 
		0, 
		0, 
		0, 
		WINDOW_W, 
		WINDOW_H, 
		False);
	XSync(display, False);
}




#endif







// delay is in useconds
static int camera_read(int type)
{
// Size of last compressed image
	int picture_size = 0;

	create_command( CMD_GETPICTURE, type, 0, 0, 0 );
	send_command();
	int result = wait_for_ack(PACKAGE_DELAY, CMD_GETPICTURE);
	

	printf("camera_read %d result=%d\n", __LINE__, result);
	if(!result) return result;

	if(wait_for_response(PACKAGE_DELAY, receive_cmd, CMD_SIZE))
	{
// Get the size of the picture
		picture_size = receive_cmd[5] << 16;
		picture_size |= receive_cmd[4] << 8;
		picture_size |= receive_cmd[3];
		printf("camera_read %d picture_size=%d\n", __LINE__, picture_size);

		int errors = 0;
		int packages = 0;
// Amount of data in a package
		int package_size = 0;
// Offset in picture data
		int picture_offset = 0;
		
		unsigned char package[PACKAGE_SIZE];
		unsigned char *picture_data = malloc(picture_size);
		int package_result = 0;
		
		while(picture_offset < picture_size && errors < MAX_ERRORS)
		{
			int fragment_size = PACKAGE_SIZE;
			if(fragment_size - PACKAGE_DATA_START - 2 + picture_offset > 
				picture_size)
			{
				fragment_size = picture_size - 
					picture_offset + 
					PACKAGE_DATA_START + 
					2;
			}

#ifndef USE_MICRO			
			send_ack(0, packages);
#endif

			printf("camera_read %d picture_offset=%d\r", __LINE__, picture_offset);
			fflush(stdout);
			package_result = wait_for_response(PACKAGE_DELAY, 
				package, 
				fragment_size);
			if(package_result)
			{
				package_size = package[3] << 8;
				package_size |= package[2];

//printf("camera_read %d package_size=%d\n", __LINE__, package_size);

				if(package_size > PACKAGE_SIZE) package_size = PACKAGE_SIZE;
				if(package_size < 0) package_size = 0;
				memcpy(picture_data + picture_offset, 
					package + PACKAGE_DATA_START,
					package_size);

				picture_offset += package_size;
				packages++;
				errors = 0;
			}
			else
			{
				errors++;
				printf("camera_read %d errors=%d\n", __LINE__, errors);
			}
		}

// Not sent by ACK micro
		send_ack( 0, LAST_JPEG_ACK );
		printf("camera_read %d done\n", __LINE__);

// Save picture data
		char string[1024];
		sprintf(string, "%s%06d.jpg", JPEG_PREFIX, picture_number);
		FILE *fd = fopen(string, "w");
		fwrite(picture_data, picture_size, 1, fd);
		fclose(fd);
		printf("camera_read %d wrote %s\n", __LINE__, string);

// Display picture in window
#ifdef USE_X11
		draw_picture(picture_data, picture_size);
#endif

		free(picture_data);

		if(errors >= MAX_ERRORS) 
			return 0;
		else
			return 1;
	}

	return 0;
}











int main()
{
	serial_fd = init_serial(SERIAL_PATH, SERIAL_BAUD, 0);

#ifdef USE_X11
	init_window();
#endif

// Compute starting picture number
	char string[1024];
	while(1)
	{
		sprintf(string, "%s%06d.jpg", JPEG_PREFIX, picture_number);
		FILE *fd = fopen(string, "r");
		if(!fd) break;
		fclose(fd);
		picture_number++;
	}

	camera_sync();

	printf("main %d\n", __LINE__);
	camera_init(CT_JPEG, PR_160x120, RESOLUTION);
	printf("main %d\n", __LINE__);
	camera_package_size(PACKAGE_SIZE);
	camera_light_freq(FT_60HZ);
	printf("main %d\n", __LINE__);

	while(1)
	{
		struct timeval start_time;
		struct timeval end_time;
		gettimeofday(&start_time, 0);
		camera_snapshot(ST_COMPRESSED, 0);
		usleep(PROCESS_DELAY);
		int result = camera_read(PT_JPEG);
		gettimeofday(&end_time, 0);
		printf("main %d %dms\n", 
			__LINE__,
			end_time.tv_sec * 1000 + end_time.tv_usec / 1000 -
			start_time.tv_sec * 1000 - start_time.tv_usec / 1000);

		if(!result)
		{
			camera_sync();
			camera_init(CT_JPEG, PR_160x120, RESOLUTION);
			camera_package_size(PACKAGE_SIZE);
			camera_light_freq(FT_60HZ);
		}
	}
}









