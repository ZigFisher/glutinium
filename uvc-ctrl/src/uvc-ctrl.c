/***************************************************************************
 *   Copyright (C) 2015 by Tse-Lun Bien                                    *
 *   allanbian@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <linux/videodev2.h>

#define UVC_DEV		"/dev/video0"

struct uvc_info {
	int val;
	int max;
	int min;
	int step;
};

void v4l2_ctrl_info(char *name, struct uvc_info *info)
{
	printf("%s: %d (min:%d, max:%d, step:%d)\n", name , 
			info->val, info->min, info->max, info->step);
}

int v4l2_ctrl_get(int fd, unsigned int id, struct uvc_info *info)
{
	struct v4l2_queryctrl query;
	struct v4l2_control ctrl;

	query.id = id;
	if (ioctl(fd, VIDIOC_QUERYCTRL, &query)) {
		return -1;
	}

	ctrl.id = id;
	if (ioctl(fd, VIDIOC_G_CTRL, &ctrl)) {
		return -1;
	}

	info->max = query.maximum;
	info->min = query.minimum;
	info->step = query.step;
	info->val = ctrl.value;

	return 0;
}

int v4l2_ctrl_set(int fd, unsigned int id, int val)
{
	struct v4l2_control ctrl;
	struct v4l2_queryctrl query; 

	query.id = id;
	if (!ioctl(fd, VIDIOC_QUERYCTRL, &query)) {
		ctrl.id = id;
		if (val > query.maximum) {
			val = query.maximum;
		}

		if (val < query.minimum) {
			val = query.minimum;
		}

		ctrl.value = val;
		if (!ioctl(fd, VIDIOC_S_CTRL, &ctrl)) {
			return 0;
		}
	}

	return -1;
}

static void print_usage(char *name)
{
	printf("Usage:  %s options [ inputfile ... ]\n", name);
	printf(
			"  -b  --brightness val       Assign brightness value.\n"
			"  -c  --contrast val         Assign contrast value.\n"
			"  -h  --hue val              Assign hue value.\n"
			"  -s  --saturation val       Assign saturation value.\n"
			"  -a  --sharpness val        Assign sharpness value.\n"
			"  -g  --gamma val            Assign gamma value.\n"
			"  -i  --gain val             Assign gain value.\n"
			"  -b  --backlight val        Assign backlight value.\n"
			"  -u  --auto                 Enable auto focus.\n"
			"  -f  --focus val            Assign absolute focus value.\n"
			"  -z  --zoom val	      Assign absolute zoom value.\n"
	      );
	exit(1);
}

int main(int argc, char *argv[])
{
	int fd;
	int ret;
	int val;
	int next_option;
	struct uvc_info info;
	const char* const short_options = "b:c:h:s:a:g:i:l:uf:z:";
	const struct option long_options[] = {
		{ "brightness",     1, NULL, 'b' },
		{ "contrast",     1, NULL, 'c' },
		{ "hue",     1, NULL, 'h' },
		{ "saturation",     1, NULL, 's' },
		{ "sharpness",     1, NULL, 'a' },
		{ "gamma",     1, NULL, 'g' },
		{ "gain",      1, NULL, 'i' },
		{ "backlight",     1, NULL, 'l' },
		{ "auto",         0, NULL, 'u' },
		{ "focus",       1, NULL, 'f' },
		{ "zoom",       1, NULL, 'z' },
		{ NULL,       0, NULL, 0   }
	};

	fd = open(UVC_DEV, O_RDWR);
	if (fd < 0) {
		printf("%s: Open %s failed!\n", argv[0], UVC_DEV);
		return 1;
	}

	do {
		next_option = getopt_long(argc, argv, short_options,
				long_options, NULL);
		switch (next_option) {
			case 'b':
				val = atoi(optarg);
				ret = v4l2_ctrl_set(fd, V4L2_CID_BRIGHTNESS, val);
				if (ret < 0) {
					printf("%s: Set Brightness failed!\n", argv[0]);
				}
				break;

			case 'c':
				val = atoi(optarg);
				ret = v4l2_ctrl_set(fd, V4L2_CID_CONTRAST, val);
				if (ret < 0) {
					printf("%s: Set Contrast failed!\n", argv[0]);
				}
				break;

			case 'h':
				val = atoi(optarg);
				ret = v4l2_ctrl_set(fd, V4L2_CID_HUE, val);
				if (ret < 0) {
					printf("%s: Set Hue failed!\n", argv[0]);
				}
				break;

			case 's':
				val = atoi(optarg);
				ret = v4l2_ctrl_set(fd, V4L2_CID_SATURATION, val);
				if (ret < 0) {
					printf("%s: Set Saturation failed!\n", argv[0]);
				}
				break;

			case 'a':
				val = atoi(optarg);
				ret = v4l2_ctrl_set(fd, V4L2_CID_SHARPNESS, val);
				if (ret < 0) {
					printf("%s: Set Sharpness failed!\n", argv[0]);
				}
				break;

			case 'g':
				val = atoi(optarg);
				ret = v4l2_ctrl_set(fd, V4L2_CID_GAMMA, val);
				if (ret < 0) {
					printf("%s: Set Gamma failed!\n", argv[0]);
				}
				break;

			case 'i':
				val = atoi(optarg);
				ret = v4l2_ctrl_set(fd, V4L2_CID_GAIN, val);
				if (ret < 0) {
					printf("%s: Set Gain failed!\n", argv[0]);
				}
				break;

			case 'l':
				val = atoi(optarg);
				ret = v4l2_ctrl_set(fd, V4L2_CID_BACKLIGHT_COMPENSATION, val);
				if (ret < 0) {
					printf("%s: Set Backlight Compensation failed!\n", argv[0]);
				}
				break;

			case 'u':
				ret = v4l2_ctrl_set(fd, V4L2_CID_FOCUS_AUTO, 1);
				if (ret < 0) {
					printf("%s: Enable Auto Focus failed!\n", argv[0]);
				}
				break;

			case 'f':
				val = atoi(optarg);

				v4l2_ctrl_set(fd, V4L2_CID_FOCUS_AUTO, 0);
				ret = v4l2_ctrl_set(fd, V4L2_CID_FOCUS_ABSOLUTE, val);
				if (ret < 0) {
					printf("%s: Set Absolute Focus failed!\n", argv[0]);
				}
				break;

			case 'z':
				val = atoi(optarg);
				ret = v4l2_ctrl_set(fd, V4L2_CID_ZOOM_ABSOLUTE, val);
				if (ret < 0) {
					printf("%s: Set Absolute Zoom failed!\n", argv[0]);
				}
				break;

			case -1:    /* Done with options.  */
				break;

			default:    /* Something else: unexpected.  */
				print_usage(argv[0]);
				abort();
		}
	} while (next_option != -1);

	if (v4l2_ctrl_get(fd, V4L2_CID_BRIGHTNESS, &info) != -1) {
		v4l2_ctrl_info("Brightness", &info);
	}

	if (v4l2_ctrl_get(fd, V4L2_CID_CONTRAST, &info) != -1) {
		v4l2_ctrl_info("Contrast", &info);
	}

	if (v4l2_ctrl_get(fd, V4L2_CID_HUE, &info) != -1) {
		v4l2_ctrl_info("Hue", &info);
	}

	if (v4l2_ctrl_get(fd, V4L2_CID_SATURATION, &info) != -1) {
		v4l2_ctrl_info("Saturation", &info);
	}

	if (v4l2_ctrl_get(fd, V4L2_CID_SHARPNESS, &info) != -1) {
		v4l2_ctrl_info("Sharpness", &info);
	}

	if (v4l2_ctrl_get(fd, V4L2_CID_GAMMA, &info) != -1) {
		v4l2_ctrl_info("Gamma", &info);
	}

	if (v4l2_ctrl_get(fd, V4L2_CID_GAIN, &info) != -1) {
		v4l2_ctrl_info("Gain", &info);
	}

	if (v4l2_ctrl_get(fd, V4L2_CID_BACKLIGHT_COMPENSATION, &info) != -1) {
		v4l2_ctrl_info("Backlight Compensation", &info);
	}

	if (v4l2_ctrl_get(fd, V4L2_CID_FOCUS_AUTO, &info) != -1) {
		if (info.val == 1) {
			printf("Auto Focus Enable\n");
		} else {
			if (v4l2_ctrl_get(fd, V4L2_CID_FOCUS_ABSOLUTE, &info) != -1) {
				v4l2_ctrl_info("Absolute Focus", &info);
			}
		}
	} else {
		if (v4l2_ctrl_get(fd, V4L2_CID_FOCUS_ABSOLUTE, &info) != -1) {
			v4l2_ctrl_info("Absolute Focus", &info);
		}
	}

	if (v4l2_ctrl_get(fd, V4L2_CID_ZOOM_ABSOLUTE, &info) != -1) {
		v4l2_ctrl_info("Absolute Zoom", &info);
	}

	close(fd);

	return 0;
}
