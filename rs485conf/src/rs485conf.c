/*
 * Copyright 2016, Grinn
 * Author: Marcin Niestroj <m.niestroj@grinn-global.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <asm-generic/ioctls.h>
#include <linux/serial.h>
#include <sys/ioctl.h>

void show_help(const char *progname)
{
	printf("usage: %s [-h] tty\n", progname);
	printf("\n");
	printf("Get/set RS485 configuration of TTY.\n");
	printf("\n");
	printf("positional arguments:\n");
	printf("  tty        TTY to get/set configuration (e.g. /dev/ttyS1)\n");
	printf("\n");
	printf("optional arguments:\n");
	printf("  -e {0,1}   Disable/enable RS485 mode\n");
	printf("  -o {0,1}   Set RTS on send low/high\n");
	printf("  -a {0,1}   Set RTS after send low/high\n");
	printf("  -r {0,1}   Set RX during TX\n");
	printf("  -d d1 d2   Set delay RTS before (d1) and after (d2) send\n");
}

void dump_conf(struct serial_rs485 *rs485conf)
{
	printf("RS485 enabled:                %s\n", (rs485conf->flags & SER_RS485_ENABLED) ? "true" : "false");
	printf("RTS on send:                  %s\n", (rs485conf->flags & SER_RS485_RTS_ON_SEND) ? "high" : "low");
	printf("RTS after send:               %s\n", (rs485conf->flags & SER_RS485_RTS_AFTER_SEND) ? "high" : "low");
	printf("RTS delay before send:        %d\n", (int) rs485conf->delay_rts_before_send);
	printf("RTS delay after send:         %d\n", (int) rs485conf->delay_rts_after_send);
	printf("Receive during sending data:  %s\n", (rs485conf->flags & SER_RS485_RX_DURING_TX) ? "true" : "false");
}

enum state {
	STATE_NONE,
	STATE_TRUE,
	STATE_FALSE
};

struct config {
	const char *tty;
	enum state enabled;
	enum state rts_on_send;
	enum state rts_after_send;
	enum state rx_during_tx;
	int delay_rts_before_send;
	int delay_rts_after_send;
};

static struct config config = {
	.delay_rts_before_send = -1,
	.delay_rts_after_send = -1,
};

#define SET_FLAGS_FROM_STATE(flags, bit, state)		\
	do {						\
		if (state) {				\
			if (state == STATE_TRUE)	\
				flags |= bit;		\
			else				\
				flags &= ~(bit);	\
		}					\
	} while (0);

void opt_fail(const char *err)
{
	fprintf(stderr, "Error: %s\n", err);
	exit(1);
}

void parse_options(int argc, char **argv)
{
	int c;

	while ((c = getopt(argc, argv, "he:o:a:r:d:")) != -1) {
		switch (c) {
		case 'h':
			show_help(argv[0]);
			exit(0);
			break;
		case 'e':
			if (!strcmp(optarg, "0"))
				config.enabled = STATE_FALSE;
			else if (!strcmp(optarg, "1"))
				config.enabled = STATE_TRUE;
			else
				opt_fail("Invalid -e argument!\n");
			break;
		case 'o':
			if (!strcmp(optarg, "0"))
				config.rts_on_send = STATE_FALSE;
			else if (!strcmp(optarg, "1"))
				config.rts_on_send = STATE_TRUE;
			else
				opt_fail("Invalid -o argument!\n");
			break;
		case 'a':
			if (!strcmp(optarg, "0"))
				config.rts_after_send = STATE_FALSE;
			else if (!strcmp(optarg, "1"))
				config.rts_after_send = STATE_TRUE;
			else
				opt_fail("Invalid -a argument!\n");
			break;
		case 'r':
			if (!strcmp(optarg, "0"))
				config.rx_during_tx = STATE_FALSE;
			else if (!strcmp(optarg, "1"))
				config.rx_during_tx = STATE_TRUE;
			else
				opt_fail("Invalid -r argument!\n");
			break;
		case 'd': {
			unsigned long int d1;
			unsigned long int d2;

			errno = 0;
			d1 = strtoul(optarg, NULL, 0);
			if (errno)
				opt_fail("Invalid -d 1st argument\n");

			d2 = strtoul(argv[optind], NULL, 0);
			if (errno)
				opt_fail("Invalid -d 2nd argument\n");

			optind++;

			config.delay_rts_before_send = d1;
			config.delay_rts_after_send = d2;
			break;
		}
		}
	}

	if (!argv[optind]) {
		show_help(argv[0]);
		exit(1);
	}
	config.tty = argv[optind];
}

int main(int argc, char *argv[])
{
	struct serial_rs485 rs485conf;
	int fd;
	int ret;

	parse_options(argc, argv);

	fd = open(config.tty, O_RDWR);
	if (fd < 0) {
		perror("Failed to open tty device\n");
		exit(1);
	}

	/* Get current configuration */
	ret = ioctl(fd, TIOCGRS485, &rs485conf);
	if (ret < 0) {
		perror("Failed to get RS485 configuration");
		exit(1);
	}

	printf("= Current configuration:\n");
	dump_conf(&rs485conf);

	if (config.enabled == STATE_NONE &&
		config.rts_on_send == STATE_NONE &&
		config.rts_after_send == STATE_NONE &&
		config.rx_during_tx == STATE_NONE &&
		config.delay_rts_before_send == -1 &&
		config.delay_rts_after_send == -1)
		goto close_fd;

	/* Set new configuration */
	SET_FLAGS_FROM_STATE(rs485conf.flags, SER_RS485_ENABLED, config.enabled);
	SET_FLAGS_FROM_STATE(rs485conf.flags, SER_RS485_RTS_ON_SEND, config.rts_on_send);
	SET_FLAGS_FROM_STATE(rs485conf.flags, SER_RS485_RTS_AFTER_SEND, config.rts_after_send);
	SET_FLAGS_FROM_STATE(rs485conf.flags, SER_RS485_RX_DURING_TX, config.rx_during_tx);
	if (config.delay_rts_before_send != -1)
		rs485conf.delay_rts_before_send = config.delay_rts_before_send;
	if (config.delay_rts_after_send != -1)
		rs485conf.delay_rts_after_send = config.delay_rts_after_send;

	printf("\n");
	printf("= New configuration:\n");
	dump_conf(&rs485conf);

	ret = ioctl(fd, TIOCSRS485, &rs485conf);
	if (ret < 0) {
		perror("Failed to set RS485 configuration");
		exit(1);
	}

	printf("\n");
	printf("= Saved configuration:\n");
	dump_conf(&rs485conf);

close_fd:
	if (close(fd) < 0) {
		perror("Failed to close tty\n");
		exit(1);
	}

	return 0;
}
