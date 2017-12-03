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

#include <fcntl.h>
#include <getopt.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <syslog.h>

#include "gpio.h"
#include "fileutil.h"

#define OPTSTRING "s:e:vDdl:"

#define OPT_SCRIPT_DIR		's'
#define OPT_DEFAULT_EDGE	'e'
#define OPT_VERBOSE		'v'
#define OPT_DETACH		'd'

// Where to look for event scripts.  Scripts in this directory
// must be named after the pin number being monitored (so,
// for example, /etc/gpio-scripts/4).
#ifndef DEFAULT_SCRIPT_DIR
#define DEFAULT_SCRIPT_DIR "/etc/gpio-scripts"
#endif

// use for converting seconds to nanoseconds.
#define NANOS 1000000000LL
#define DEBOUNCE_INTERVAL 100000L

char *script_dir = DEFAULT_SCRIPT_DIR;
char *logfile = NULL;
int default_edge = EDGE_BOTH;
int detach       = 0;

int loglevel = LOG_INFO;

// This will hold the list of pins to monitor generated from
// command line arguments.
struct pin *pins = NULL;
int num_pins = 0;

void usage (FILE *out) {
	fprintf(out, "gpio-watch: usage: gpio-watch [-l logfile] [-s script_dir] [-e default_edge] [-dv] pin[:edge] [...]\n");
}

// Run a script in response to an event.
void run_script (int pin, int value, int switch_interval) {
	char *script_path,
	     pin_str[GPIODIRLEN],
	     value_str[2],
	     interval_str[10];
	int script_path_len;
	pid_t pid;
	int status;

	script_path_len = strlen(script_dir)
			+ GPIODIRLEN
			+ 2;
	script_path = (char *)malloc(script_path_len);

	snprintf(script_path, script_path_len,
			"%s/%d", script_dir, pin);

	if (! is_file(script_path)) {
		syslog(LOG_WARNING,"pin %d: script \"%s\" does not exist",
				pin, script_path);
		return;
	}

	snprintf(pin_str, GPIODIRLEN, "%d", pin);
	sprintf(value_str, "%d", value);
	sprintf(interval_str, "%d", switch_interval);

	syslog(LOG_INFO,"pin %d state %d interval %d: running script %s", pin, value, switch_interval, script_path);

	if (0 == (pid = fork())) {
		int res;
		res = execl(script_path, script_path, pin_str, value_str, interval_str, (char *)NULL);
		if (-1 == res) exit(255);
	}

	wait(&status);

	if (WIFEXITED(status)) {
		if (0 != WEXITSTATUS(status)) {
			syslog(LOG_WARNING,"pin %d: event script exited with status = %d",
					pin, WEXITSTATUS(status));
		}
	} else if (WIFSIGNALED(status)) {
		syslog(LOG_WARNING,"pin %d: event script exited due to signal %d",
				pin, WTERMSIG(status));
	}

	free(script_path);

}

// Loop forever monitoring pins for activity.
int watch_pins() {
	struct pollfd *fdlist;
	int i;
	char *pin_path;
	int pin_path_len;
	char valbuf[3];
	struct timespec ts;
	int switch_interval;

	unsigned char switch_state[num_pins];
	long long now,
	     down_at[num_pins],
	     up_at[num_pins];

	valbuf[2] = '\0';
	memset(switch_state, 0, num_pins);

	pin_path_len = strlen(GPIO_BASE) + GPIODIRLEN + strlen("value") + 3;
	pin_path = (char *)malloc(pin_path_len);

	fdlist = (struct pollfd *)malloc(sizeof(struct pollfd) * num_pins);
	for (i=0; i<num_pins; i++) {
		int fd;

		up_at[i]=down_at[i]=now;

		snprintf(pin_path, pin_path_len,
				"%s/gpio%d/value", GPIO_BASE, pins[i].pin);
		fd = open(pin_path, O_RDONLY);
		read(fd, valbuf, 2);
		fdlist[i].fd = fd;
		fdlist[i].events = POLLPRI;
	}

	syslog(LOG_INFO,"starting to monitor for gpio events");

	while (1) {
		int err;

		err = poll(fdlist, num_pins, -1);
		if (-1 == err) {
			perror("poll");
			exit(1);
		}

		for (i=0; i<num_pins; i++) {
			if (fdlist[i].revents & POLLPRI) {
				syslog(LOG_DEBUG,"pin %d: received event",
						pins[i].pin);
				lseek(fdlist[i].fd, 0, SEEK_SET);
				read(fdlist[i].fd, valbuf, 2);

				// for pins use 'switch' edge mode, we only trigger
				// an event when we receive the '1' event more than
				// DEBOUNCE_INTERVAL nanoseconds after the '0' event.
  				if (EDGE_SWITCH == pins[i].edge) {
					clock_gettime(CLOCK_MONOTONIC, &ts);
					now = ts.tv_sec * NANOS + ts.tv_nsec;

					if (switch_state[i] == 0 && valbuf[0] == '1') {
						switch_interval = now - up_at[i];
						switch_state[i] = 1;
						down_at[i] = now;
					} else if (switch_state[i] == 1 && valbuf[0] == '0') {
						if (now - down_at[i] > DEBOUNCE_INTERVAL) {
							switch_interval = now - down_at[i];
							switch_state[i] = 0;
							up_at[i] = now;
							goto run_script;
						}
					}

					continue;
  				} else {
  					switch_interval = 0;
  				}

run_script:
				run_script(pins[i].pin,
						valbuf[0] == '1' ? 1 : 0,
						switch_interval/(1000*1000));
			}
		}
	}
}

int main(int argc, char **argv) {
	struct pollfd *fdlist;
	int numfds = 0;
	int ch;
	int i;

	while (-1 != (ch = getopt(argc, argv, OPTSTRING))) {
		switch (ch) {
			case OPT_DETACH:
				detach = 1;
				break;
			case OPT_SCRIPT_DIR:
				script_dir = strdup(optarg);
				break;
			case OPT_DEFAULT_EDGE:
				if (-1 == (default_edge = parse_edge(optarg))) {
					fprintf(stderr, "error: invalid edge value: %s\n", optarg);
					exit(1);
				}
				break;
			case OPT_VERBOSE:
				loglevel += 1;
				break;

			case '?':
				usage(stderr);
				exit(2);
		}
	}

	if (logfile) {
		int fd;
		if (-1 == (fd = open(logfile, O_WRONLY|O_CREAT|O_APPEND, 0644))) {
			syslog(LOG_ERR,"failed to open logfile %s", logfile);
			exit(1);
		}

		close(1);
		close(2);

		dup(fd);
		dup(fd);
	}

	if (! is_dir(script_dir)) {
		syslog(LOG_ERR,"error: script directory \"%s\" does not exist.",
				script_dir);
		exit(1);
	}

	for (i=optind; i<argc; i++) {
		char *pos,
		     *pinspec;
		struct pin p;

		pinspec = strdup(argv[i]);
		pos = strchr(pinspec, ':');

		if (pos) {
			*pos = '\0';
			pos++;
			p.pin = atoi(pinspec);
			if (-1 == (p.edge = parse_edge(pos))) {
				fprintf(stderr, "error: unknown edge spec: %s\n",
						argv[i]);
				exit(1);
			}
		} else {
			p.pin = atoi(pinspec);
			p.edge = default_edge;
		}

		free(pinspec);

		num_pins++;
		pins = realloc(pins, sizeof(struct pin) * num_pins);
		pins[num_pins-1] = p;
	}

	for (i=0; i<num_pins; i++) {
		pin_export(pins[i].pin);
		pin_set_edge(pins[i].pin, pins[i].edge);
		pin_set_direction(pins[i].pin, DIRECTION_IN);
	}

	if (detach) daemon(1, logfile ? 1: 0);

	watch_pins();

	return 0;
}

