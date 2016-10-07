/* Name: nrf24le1 Downloader USBasp 1.0
 основан на проекте https://github.com/derekstavis/nrf24le1-libbcm2835 .
страничка проекта http://homes-smart.ru/index.php/oborudovanie/bez-provodov-2-4-ggts/55-programmirovanie-nrf24le1-cherez-usbasp
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <usb.h>        /* this is libusb */
#include "opendevice.h" /* common code moved to separate module */
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#if HIDMODE /* device's VID/PID and names */
#include "usbconfigHID.h"
#else
#include "usbconfig.h"
#endif


void nrf_save_data(uint8_t * buf, uint16_t count, uint8_t * fname)
{
	size_t size_writed = 0, idx = 0;
	FILE * fd;

	fd = fopen(fname, "w");

	while(idx < count)
	{
		size_writed = fwrite(&buf[idx], 1, (count - idx), fd);
		idx = size_writed + idx;
	}
	fclose(fd);
}

void nrf_restore_data(uint8_t * buf, uint16_t count, uint8_t * fname)
{
	FILE * fd;
	fd = fopen(fname, "r");
	fread(buf, 1, count, fd);
	fclose(fd);
}
// ------
static void usage(char *name)
{
    fprintf(stderr, "Прошивка nRF24LE1 через USBasp.  http://homes-smart.ru/\nОснован на https://github.com/derekstavis/nrf24le1-libbcm2835\nusage:\n");
    fprintf(stderr, " %s test -информация\n", name);
    fprintf(stderr, " %s write -запись прошивки файла main.bin.\n", name);
    fprintf(stderr, " %s read -чтение прошивки в файл main-dump.bin.\n", name);
}



usb_dev_handle      *handle = NULL;
char                buffer[32];
int                 cnt;

#include "wiring.c"
#include "nrf24le1.h"
#include "nrf24le1.c"

//-----------------------------------------------------------
int main(int argc, char **argv)
{
//usb_dev_handle      *handle = NULL;
const unsigned char rawVid[2] = {USB_CFG_VENDOR_ID}, rawPid[2] = {USB_CFG_DEVICE_ID};
char                vendor[] = {USB_CFG_VENDOR_NAME, 0}, product[] = {USB_CFG_DEVICE_NAME, 0};
//char                buffer[8];

int                  vid, pid;

    usb_init();
    if(argc < 2){   /* we need at least one argument */
        usage(argv[0]);
        exit(1);
    }
    /* compute VID/PID from usbconfig.h so that there is a central source of information */
    vid = rawVid[1] * 256 + rawVid[0];
    pid = rawPid[1] * 256 + rawPid[0];
    /* The following function is in opendevice.c: */
    if(usbOpenDevice(&handle, vid, vendor, pid, product, NULL, NULL, NULL) != 0){
        fprintf(stderr, "Could not find USB device \"%s\" with vid=0x%x pid=0x%x\n", product, vid, pid);
        exit(1);
    }
    
    
    	uint8_t bufread[17000];
	unsigned long off = 0;
	size_t count =16384;

	memset(bufread, 0, sizeof(bufread));

	nrf24le1_init();

	enable_program(1);

	if(strcasecmp(argv[1], "test") == 0) da_test_show();
	   
	else
	if(strcasecmp(argv[1], "write") == 0){

	nrf_restore_data(bufread, count, "./main.bin");
	uhet_write(bufread, 16384, &off);


	} else 
	if(strcasecmp(argv[1], "read") == 0){


	memset(bufread, 0, sizeof(bufread));
	uhet_read(bufread, count, &off);

	nrf_save_data(bufread, count, "./main-dump.bin");

	}

	//da_erase_all_store();

	enable_program(0);

	wiring_destroy();
    
    
    
    usb_close(handle);
    return 0;
}
