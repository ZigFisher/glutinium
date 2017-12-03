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

#ifndef _GPIO_H
#define _GPIO_H

#define EDGE_NONE 0
#define EDGE_RISING 1
#define EDGE_FALLING 2
#define EDGE_BOTH 3
#define EDGE_SWITCH 4
#define EDGESTRLEN 8

#define DIRECTION_IN 0
#define DIRECTION_OUT 1

#define EDGESTRLEN 8
#define DIRSTRLEN 4
#define GPIODIRLEN 8

#ifndef GPIO_BASE
#define GPIO_BASE "/sys/class/gpio"
#endif

struct pin {
	int pin;
	int edge;
};

int parse_direction(const char *direction);
int parse_edge(const char *edge);
void pin_export(int pin);
int pin_set_edge(int pin, int edge);
int pin_set_direction(int pin, int direction);

#endif // _GPIO_H

