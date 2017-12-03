/*
 * gpio-watch, a tool for running scripts in response to gpio events
 * Copyright (C) 2014 Lars Kellogg-Stedman <lars@oddbit.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "gpio.h"
#include "fileutil.h"

// Parse a string ("in", "out") and return
// the corresponding DIRECTION_* constant, or -1 if the string
// is invalid.
int parse_direction(const char *direction) {
	if (0 == strncmp(direction, "in", DIRSTRLEN))
		return DIRECTION_IN;
	else if (0 == strncmp(direction, "out", DIRSTRLEN))
		return DIRECTION_OUT;
	else
		return -1;
}

// Parse a string ("rising", "falling", "both") and return
// the corresponding EDGE_* constant, or -1 if the string
// is invalid.
int parse_edge(const char *edge) {
	if (0 == strncmp(edge, "rising", EDGESTRLEN))
		return EDGE_RISING;
	else if (0 == strncmp(edge, "falling", EDGESTRLEN))
		return EDGE_FALLING;
	else if (0 == strncmp(edge, "both", EDGESTRLEN))
		return EDGE_BOTH;
	else if (0 == strncmp(edge, "switch", EDGESTRLEN))
		return EDGE_SWITCH;
	else if (0 == strncmp(edge, "none", EDGESTRLEN))
		return EDGE_NONE;
	else
		return -1;
}

// Export a pin by writing to /sys/class/gpio/export.
void pin_export(int pin) {
	char *export_path,
	     *pin_path;
	int export_path_len,
	    pin_path_len;

	export_path_len = strlen(GPIO_BASE) + strlen("export") + 3;
	export_path = (char *)malloc(export_path_len);
	snprintf(export_path, export_path_len-1, "%s/export", GPIO_BASE);

	pin_path_len = strlen(GPIO_BASE) + GPIODIRLEN + 3;
	pin_path = (char *)malloc(pin_path_len);
	snprintf(pin_path, pin_path_len-1, "%s/gpio%d", GPIO_BASE, pin);

	if (! is_dir(pin_path)) {
		FILE *fp;
		int tries = 0;

		fp = fopen(export_path, "w");
		fprintf(fp, "%d\n", pin);
		fclose(fp);

		// gpio directories are initially owned by 'root'.  If you have
		// udev rules that change this, it may take a moment for those 
		// changes to happen.
		sleep(1);
	}

end:
	free(pin_path);
	free(export_path);
}
 
// Set which signal edges to detect.
int pin_set_edge(int pin, int edge) {
	char *pin_path;
	int pin_path_len;
	FILE *fp;

	pin_path_len = strlen(GPIO_BASE) + GPIODIRLEN + strlen("edge") + 4;
	pin_path = (char *)malloc(pin_path_len);
	snprintf(pin_path, pin_path_len - 1,
			"%s/gpio%d", GPIO_BASE, pin);

	if (! is_dir(pin_path)) {
		exit(1);
	}

	snprintf(pin_path, pin_path_len,
			"%s/gpio%d/edge", GPIO_BASE, pin);

	fp = fopen(pin_path, "w");
	if (! fp) {
		exit(1);
	}

	if (EDGE_RISING == edge)
		fprintf(fp, "rising\n");
	else if (EDGE_FALLING == edge)
		fprintf(fp, "falling\n");
	else if (EDGE_BOTH == edge || EDGE_SWITCH == edge)
		fprintf(fp, "both\n");
	else {
		exit(1);
	}

	fclose(fp);
	free(pin_path);
}

// Set pin as input or output.
int pin_set_direction(int pin, int direction) {
	char *pin_path;
	int pin_path_len;
	FILE *fp;

	pin_path_len = strlen(GPIO_BASE) + GPIODIRLEN + strlen("direction") + 3;
	pin_path = (char *)malloc(pin_path_len);
	snprintf(pin_path, pin_path_len,
			"%s/gpio%d", GPIO_BASE, pin);

	if (! is_dir(pin_path)) {
		exit(1);
	}

	snprintf(pin_path, pin_path_len,
			"%s/gpio%d/direction", GPIO_BASE, pin);

	fp = fopen(pin_path, "w");
	if (DIRECTION_IN == direction)
		fprintf(fp, "in\n");
	else if (DIRECTION_OUT == direction)
		fprintf(fp, "out\n");
	else {
		exit(1);
	}

	fclose(fp);
	free(pin_path);
}

