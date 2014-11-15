/*
* barcode - simple reader from optical scanner
*
* Copyright (C) 2012-2013 Vlad Moskovets vlad.moskovets@gmail.com
* Copyright (C) 2013-2014 Zig Fisher flyrouter@gmail.com
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
*
* For usage add this example string to /etc/inetd.conf:
*
* 10444   stream   tcp   nowait   root   /usr/sbin/barcode   barcode /dev/input/event0
*
*/

#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>

#define LFTIMEOUT (20 * 1000) // timeout in microseconds: 0.02 sec
#define USLEEP (10 * 1000)
#define debug if(0)printf

char scancode_to_char(int foo) {

	char bar;
	switch (foo) {
		case KEY_0: bar = '0'; break;
		case KEY_1: bar = '1'; break;
		case KEY_2: bar = '2'; break;
		case KEY_3: bar = '3'; break;
		case KEY_4: bar = '4'; break;
		case KEY_5: bar = '5'; break;
		case KEY_6: bar = '6'; break;
		case KEY_7: bar = '7'; break;
		case KEY_8: bar = '8'; break;
		case KEY_9: bar = '9'; break;

		case KEY_A: bar = 'A'; break;
		case KEY_B: bar = 'B'; break;
		case KEY_C: bar = 'C'; break;
		case KEY_D: bar = 'D'; break;
		case KEY_E: bar = 'E'; break;
		case KEY_F: bar = 'F'; break;
		case KEY_G: bar = 'G'; break;
		case KEY_H: bar = 'H'; break;
		case KEY_I: bar = 'I'; break;
		case KEY_J: bar = 'J'; break;
		case KEY_K: bar = 'K'; break;
		case KEY_L: bar = 'L'; break;
		case KEY_M: bar = 'M'; break;
		case KEY_N: bar = 'N'; break;
		case KEY_O: bar = 'O'; break;
		case KEY_P: bar = 'P'; break;
		case KEY_Q: bar = 'Q'; break;
		case KEY_R: bar = 'R'; break;
		case KEY_S: bar = 'S'; break;
		case KEY_T: bar = 'T'; break;
		case KEY_U: bar = 'U'; break;
		case KEY_V: bar = 'V'; break;
		case KEY_W: bar = 'W'; break;
		case KEY_X: bar = 'X'; break;
		case KEY_Y: bar = 'Y'; break;
		case KEY_Z: bar = 'Z'; break;
		case KEY_ENTER: bar = '\n'; break;
		default:
				bar = 0;
				debug("<DEBUG: foo: %d>\n", foo);

	}
	return bar;
}

int main(int argc, char **argv)
{
	int fd;
	struct input_event events[64];
	struct timeval last_read_at, now;
	if(argc < 2) {
		printf("usage: %s /dev/input/eventX \n", argv[0]);
		return 1;
	}
	fd = open(argv[1], O_RDONLY | O_NOCTTY | O_NDELAY);

	//printf("sizeof(events): %d\n", sizeof(events));
	while (1)
	{
		struct input_event *event;
		int i = 0;
		int readed = read(fd, &events, sizeof(events));
		int count = readed / (sizeof(struct input_event));
		debug("<DEBUG: readed: %d, count: %d>\n", readed, count);
		if ( readed > 0 ) {

			for (i = 0; i < count; i++) {
				event = &events[i];
				//debug("<DEBUG: type: %d, code: %d, value: %d>\n", event->type, event->code, event->value );
				if (event->type == EV_KEY && event->value == 0 && event->code <= KEY_COMMA) {
					char ch = scancode_to_char(event->code);
					if (ch) {
						putchar( ch );
					}
				}
			}
			gettimeofday (&last_read_at, NULL);
			fflush(stdout);
		} else if ( readed == -1 && errno == EAGAIN ) {
			debug("<DEBUG: timeout>\n");

			gettimeofday (&now, NULL);
			if (last_read_at.tv_sec && (now.tv_sec  - last_read_at.tv_sec) * 1000000 + now.tv_usec - last_read_at.tv_usec > LFTIMEOUT) {
				debug("<DEBUG: print LF>\n");
				last_read_at.tv_sec = 0;
				putchar('\n');
			}

			usleep(USLEEP);
		} else {
			perror("read");
			exit(1);
			//debug("<DEBUG: unknown readed: %d >\n", readed);
		}
	}
}
