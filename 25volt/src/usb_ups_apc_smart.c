/*
**  25volt - A lightweight tool for monitoring ups POWERCOM Imperial IMD-1025AP and maybe other for FreeBSD and Linux.
**  Copyright (C) 2009 Dmitry Schedrin <dmx@dmx.org.ru>
**
**  Corrected for Powercom WOW-xxxU by c61 <mail@c61.su> 2013
**  From program version 1.0.4 Powercom BNT, IMPERIAL, SKP, WOW series are supported
**  (see -v and -p options)
**
**  Corrected for APC Smart UPS by c61 <mail@c61.su> 2013
**  DO NOT USE FOR POWERCOM UPSes !!!
**
**  This program is free software; you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation; either version 2 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <usb.h>
#include <unistd.h>

//#define DEBUG

#define HID_GET_REPORT 			0x01
#define HID_INPUT_REPORT 		0x01

#define HID_INTERFACE 			0x00
#define HID_CONFIGURATION 		0x00

#define IMD_VID 			0x051d
#define IMD_PID				0x0002

#define USBRQ_HID_GET_REPORT    	0x01
#define USBRQ_HID_SET_REPORT    	0x09

#define USB_HID_REPORT_TYPE_FEATURE 	3

#define UPS_REPORT_VOLTAGE_IN 		32
#define UPS_REPORT_VOLTAGE_OUT		43
#define UPS_REPORT_FREQUENCY_IN		42
#define UPS_REPORT_FREQUENCY_OUT	42
#define UPS_REPORT_LOAD			44
#define UPS_RUN_TIME_TO_EMPTY		25//13
#define UPS_REPORT_CAPACITY		24//12
#define UPS_REPORT_STATUS		7//51

//#define PRINT_STATUS

#define CHARGING			1
#define DISCHARGING			(1 << 1)
#define ACPRESENT			(1 << 2)
#define BATTERYPRESENT			(1 << 3)
#define BELOWREMAININGCAPACITYLIMIT	(1 << 4)
#define SHUTDOWNREQUESTED 		(1 << 5)
#define SHUTDOWNIMMINENT		(1 << 6)
#define REMAININGTIMELIMITEXPIRED 	(1 << 7)
#define COMMUNICATIONLOST		(1 << 8)
#define NEEDREPLACEMENT			(1 << 9)
#define OVERLOAD			(1 << 10)
#define VOLTAGENOTREGULATED 		(1 << 11)

uint16_t status;

char version[] ="1.0.5";

int vendor = IMD_VID, product = IMD_PID;

usb_dev_handle * usb_open_imd(void);

int tflag = 0, hflag = 0, bflag = 0, help = 0, detach = 0, attach = 0, skip = 0, ch;

unsigned char status_human_str[][30] = {
	"Charging:",
	"Discharging:",
	"AC present:",
	"Battery present:",
	"Below cap lim:",
	"Remain time exp:",
	"Need replace BAT:",
	"Voltage not regulated:",
	"Shutdown requested:",
	"Shutdown imminent:",
	"Communication lost:",
	"Overload:"
};

unsigned char status_str[][30] = {
	"charging:",
	"discharging:",
	"ac_present:",
	"battery_present:",
	"below_capacity_limit:",
	"remain_time_expired:",
	"need_replace_battery:",
	"voltage_not_regulated:",
	"shutdown_requested:",
	"shutdown_imminent:",
	"communication_lost:",
	"overload:"
};

unsigned char status_bash_str[][30] = {
	"charging=",
	"discharging=",
	"ac_present=",
	"battery_present=",
	"below_capacity_limit=",
	"remain_time_expired=",
	"need_replace_battery=",
	"voltage_not_regulated=",
	"shutdown_requested=",
	"shutdown_imminent=",
	"communication_lost=",
	"overload="
};

usb_dev_handle* usb_open_imd(void) {

	struct usb_bus *bus;
	struct usb_device *dev;

	usb_find_busses();
	usb_find_devices();

	for (bus = usb_get_busses(); bus; bus = bus->next) {
		for (dev = bus->devices; dev; dev = dev->next) {
			if ((dev->descriptor.idVendor == vendor)
			    && (dev->descriptor.idProduct == product)) {
				return usb_open(dev);
			}
		}
	}
	return NULL;
}

void print_data(unsigned char *tmp, int ret) {

	int i;

	for (i=0; i<ret; i++) {
		printf("%.2x ", tmp[i]);
	}
	printf("\n");
}

uint16_t get_report16(usb_dev_handle *hdev, uint8_t report) {

	int16_t data;
	int ret;
	unsigned char tmp[1024];

	ret = usb_control_msg(hdev,  0xa1 , 0x01, (0x03 << 8) | report, 0, tmp, 1024, 5000);

	if (ret < 0) {
		printf("retrieving hid report failed\n");
		return -1;
	} else {
		#ifdef DEBUG
		printf("retrieving hid report succeeded, read %d bytes\n", ret);
		print_data(tmp, ret);
		#endif
		if(ret==2) {
			data = tmp[1];
		} else if(ret==3) {
			data = tmp[1]|(tmp[2]<<8);
		} else {
			data = 0;
		}
		return data;
	}
}

uint8_t get_report8(usb_dev_handle *hdev, uint8_t report) {

	int8_t data;
	int ret;
	unsigned char tmp[1024];

	ret = usb_control_msg(hdev,  0xa1 , 0x01, (0x03 << 8) | report, 0, tmp, 1024, 5000);

	if (ret < 0) {
		printf("retrieving hid report failed\n");
		return -1;
	} else {
		#ifdef DEBUG
		printf("retrieving hid report succeeded, read %d bytes\n", ret);
		print_data(tmp, ret);
		#endif
		if(ret==2) {
			data = tmp[1];
		} else {
			data = 0;
		}
		return data;
	}
}

uint8_t send_test(usb_dev_handle *hdev) {

	int8_t data;
	int ret;
	unsigned char tmp[2] = { 0x16, 0x01 };

	ret = usb_control_msg(hdev,  0x21 , 0x09, (0x03 << 8) | 0x16, 0, tmp, 2, 5000);

	if (ret < 0) {
		printf("retrieving hid report failed\n");
		return -1;
	} else {
		#ifdef DEBUG
		printf("retrieving hid report succeeded, read %d bytes\n", ret);
		print_data(tmp, ret);
		#endif
		if(ret==2) {
			data = tmp[1];
		} else {
			data = 0;
		}
		return data;
	}
}

uint8_t send_boff(usb_dev_handle *hdev) {

	int8_t data;
	int ret;
	unsigned char tmp[2] = { 0x14, 0x00 };

	ret = usb_control_msg(hdev,  0x21 , 0x09, (0x03 << 8) | 0x14, 0, tmp, 2, 5000);

	if (ret < 0) {
		printf("retrieving hid report failed\n");
		return -1;
	} else {
		#ifdef DEBUG
		printf("retrieving hid report succeeded, read %d bytes\n", ret);
		print_data(tmp, ret);
		#endif
		if(ret==2) {
			data = tmp[1];
		} else {
			data = 0;
		}
		return data;
	}
}

int main(int argc, char ** argv) {

	unsigned char tmp[1024];
	int i, ret, j;
	usb_dev_handle *hdev = NULL;
	unsigned char onoff[2][4] = {"off\0", "on\0"};

	while ((ch = getopt(argc, argv, "ustbv:p:dah?")) != EOF) {
		switch (ch) {
			case 'v':	vendor = strtol(optarg,NULL,16);	break;
			case 'p':	product = strtol(optarg,NULL,16);	break;
			case 'u':	hflag = 1;	break;
			case 's':	hflag = 2;	break;
			case 't':	tflag = 1;	skip = 1;	break;
			case 'b':	bflag = 1;	skip = 1;	break;
			case 'd':	detach = 1;	skip = 1;	break;
			case 'a':	attach = 1;	skip = 1;	break;
			case 'h':
			case '?':	help = 1;	break;
			default:	break;
		}
	}

	if(help)
	{
		printf("USB UPS APC Smart-UPS monitoring, v.%s\n", version);
		printf(" no args - get and show ups data\n");
		printf(" -u      - get and show unformatted ups data\n");
		printf(" -s      - get and show UPS data for bash script\n");
		printf(" -t      - test ups\n");
		printf(" -b      - switch off beeper (audible alarm control; experimental)\n");
		printf(" -d      - detach kernle driver before use interface\n");
		printf(" -a      - attach kernle driver after use interface\n");
		printf(" -v VID  - vendor ID (default: 051d - APC)\n");
		printf(" -p PID  - product ID (default: 0002 - APC Smart UPS)\n");
		printf(" -h -?   - this help\n");
		exit(0);
	}

	//usb_set_debug(4);
	usb_init();

	if ((hdev = usb_open_imd()) == NULL) {
		printf("open failed\n");
		exit(1);
	}

#ifdef LIBUSB_HAS_DETACH_KERNEL_DRIVER_NP
	if (detach) {
	    if(usb_detach_kernel_driver_np(hdev, 0) < 0) {
		; //fprintf(stderr, "Warning: could not detach kernel driver: %s\n", usb_strerror());
	    }
	}
#endif 

	if (usb_claim_interface(hdev, HID_INTERFACE) < 0) {
		printf("warning: claim interface failed\n");
	}

	ret = usb_control_msg(hdev, USB_ENDPOINT_IN | USB_TYPE_STANDARD |
			      USB_RECIP_INTERFACE, USB_REQ_GET_DESCRIPTOR,
			 (USB_DT_HID << 8), HID_INTERFACE, tmp, 1024, 5000);
	if (ret < 0) {
		printf("retrieving hid descriptor failed\n");
		ret = 1;
		goto release;
	} else {
		#ifdef DEBUG
		printf("retrieving hid descriptor succeeded, read %d bytes\n", ret);
		print_data(tmp, ret);
		#endif
	}

	//USB_TYPE_CLASS | USB_RECIP_DEVICE | USB_ENDPOINT_IN, USBRQ_HID_GET_REPORT,

	// 09 21 00 01 00 01 22 d0 02

	// dummy read - clear all
	for(j=0; j<8; j++) {
		ret = get_report16(hdev, UPS_REPORT_VOLTAGE_IN);
	}
	//ret = usb_clear_halt(hdev,USB_ENDPOINT_IN);
	//ret = usb_clear_halt(hdev,USB_ENDPOINT_OUT);

	if (tflag) {
		printf("Test: ");
		ret = send_test(hdev);
		if (ret < 0) {
			printf("Failed\n\n");
			ret = 1;
			goto release;
		} else {
			printf("OK\n\n");
			sleep(1);
		}
	}

	if (bflag) {
		printf("Switch OFF Audible Alarm Control: ");
		ret = send_boff(hdev);
		if (ret < 0) {
			printf("Failed\n\n");
			ret = 1;
			goto release;
		} else {
			printf("OK\n\n");
			sleep(1);
		}
	}

	if (hflag == 1) {
		printf("%s %d\n", "voltage_in:", get_report16(hdev, UPS_REPORT_VOLTAGE_IN)/10);
		printf("%s %d\n", "voltage_out:", get_report16(hdev, UPS_REPORT_VOLTAGE_OUT)/10);
		printf("%s %d\n", "frequency_out:", get_report16(hdev, UPS_REPORT_FREQUENCY_OUT)/100);
		printf("%s %d\n", "load:", get_report16(hdev, UPS_REPORT_LOAD)/10);
		printf("%s %d\n", "capacity:", get_report16(hdev, UPS_REPORT_CAPACITY)/10);
		printf("%s %d\n", "run_time_to_empty:", get_report16(hdev, UPS_RUN_TIME_TO_EMPTY));
#ifdef PRINT_STATUS
		status = get_report16(hdev, UPS_REPORT_STATUS);

		for (j=0; j<12; j++) {
			printf("%s %s\n", status_str[j], onoff[status & (1 << j) && 1]);
		}
#endif
	} else if (hflag == 2) {
		printf("%s%d\n", "voltage_in=", get_report16(hdev, UPS_REPORT_VOLTAGE_IN)/10);
		printf("%s%d\n", "voltage_out=", get_report16(hdev, UPS_REPORT_VOLTAGE_OUT)/10);
		printf("%s%d\n", "frequency_out=", get_report16(hdev, UPS_REPORT_FREQUENCY_OUT)/100);
		printf("%s%d\n", "load=", get_report16(hdev, UPS_REPORT_LOAD)/10);
		printf("%s%d\n", "capacity=", get_report16(hdev, UPS_REPORT_CAPACITY)/10);
		printf("%s%d\n", "run_time_to_empty=", get_report16(hdev, UPS_RUN_TIME_TO_EMPTY));
#ifdef PRINT_STATUS
		status = get_report16(hdev, UPS_REPORT_STATUS);

		for (j=0; j<12; j++) {
			printf("%s%d\n", status_bash_str[j], status & (1 << j) && 1);
		}
#endif
	} else {
	    if(!skip) {
		printf("%-22s %d V\n", "Voltage in:", get_report16(hdev, UPS_REPORT_VOLTAGE_IN)/10);
		printf("%-22s %d V\n", "Voltage out:", get_report16(hdev, UPS_REPORT_VOLTAGE_OUT)/10);
		printf("%-22s %d Hz\n", "Frequency out:", get_report16(hdev, UPS_REPORT_FREQUENCY_OUT)/100);
		printf("%-22s %d %%\n", "Load:", get_report16(hdev, UPS_REPORT_LOAD)/10);
		printf("%-22s %d %%\n", "Capacity:", get_report16(hdev, UPS_REPORT_CAPACITY)/10);
		printf("%-22s %d s\n", "Run time to empty:", get_report16(hdev, UPS_RUN_TIME_TO_EMPTY));
#ifdef PRINT_STATUS
		printf("\n");

		status = get_report16(hdev, UPS_REPORT_STATUS);

		for (j=0; j<12; j++) {
			printf("%-22s %s\n", status_human_str[j], onoff[status & (1 << j) && 1]);
		}
#endif
	    }
	}

	ret = 0;
release:;
	if (usb_release_interface(hdev, HID_INTERFACE) < 0) {
		printf("releasing interface failed\n");
		ret = 1;
	}

#ifdef LIBUSB_HAS_ATTACH_KERNEL_DRIVER_NP
	if (attach) {
	    if(usb_attach_kernel_driver_np(hdev, 0) < 0) {
		; //fprintf(stderr, "Warning: could not attach kernel driver: %s\n", usb_strerror());
	    }
	}
#endif 
	usb_close(hdev);

	exit(ret ? 1 : 0);
}
