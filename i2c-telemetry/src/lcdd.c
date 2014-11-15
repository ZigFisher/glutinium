/* Simple daemon to communicate with Newhaven I2C LCD module.
 *
 * This is a trivial interface for Newhaven LCD module connected to a Linux machine
 * over an I2C interface. Any standard Linux I2C bus should work (including USB I2C
 * adapters that correctly hook into the i2c kernel driver framework).
 *
 * Opens a named pipe at /dev/lcd which can be written to by other applications.
 * Text written to this file will appear on the LCD.
 * Send a bell ('\b')to clear the screen.
 *
 * Tested with NHD-0420D3Z-FL-GBW (Digikey part number NHD-0420D3Z-FL-GBW-ND).
 * http://www.newhavendisplay.com/specs/NHD-0420D3Z-FL-GBW.pdf
 * http://www.newhavendisplay.com/index.php?main_page=product_info&cPath=253&products_id=922
 *
 * FIXME how to do other commands etc.
 * FIXME some rate limiting should be added... the I2C controller on the LCD
 * module gets confused if you write to it too fast.
 *
 * Hardware note:
 * To enable I2C mode of LCD, Pads of R1 must be jumpered on back of LCD
 * See the manual for the LCD module.
 *
 * ./lcdd -a 0x47 -d /dev/i2c-4 -f /dev/lcd -n
 *
 * Hugo Vincent, June 2009 - https://github.com/hugovincent/i2c-lcdd
 * Daemonization based on code by Peter Lombardo <peter@lombardo.info>, 2006
 */


#define _GNU_SOURCE // for strsignal()

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <errno.h>
#include <syslog.h>
#include <assert.h>
#include <signal.h>
#include <poll.h>
#include <setjmp.h>
#include <linux/i2c-dev.h>

#define DAEMON_NAME "lcdd"
#define PID_FILE "/var/run/lcdd.pid"

#define LCD_ADDR 0x27

//#define DEBUG

static int daemonize = 1, running = 1, have_lcd = 1;
static jmp_buf my_jmp_buf;

#define LOG_MSG(format, ...) if (daemonize == 1) \
											syslog(LOG_INFO, format, ##__VA_ARGS__); \
else \
printf("i2c-lcdd: " format "\n", ##__VA_ARGS__);

void usage(int argc, char **argv)
{
	if (argc >=1)
	{
		printf("Usage: %s -a 0x50 -d /dev/i2c-3 -f /dev/lcd -n -?\n", argv[0]);
		printf("  Options:\n");
		printf("     -a\tI2C slave address (in hex) for LCD module\n");
		printf("     -d\tI2C device\n");
		printf("     -f\tnamed pipe for input\n");
		printf("     -n\tDon't daemonize.\n");
		printf("     -?\tShow this help screen.\n");
	}
}

void signal_handler(int sig)
{ 
	LOG_MSG("received signal %s - exiting", strsignal(sig));
	running = 0;
}

void i2c_send(int fd, uint8_t *buf, int count)
{
	if (have_lcd)
	{
		if (write(fd, buf, count) != count)
		{
			LOG_MSG("error writing to I2C: %s", strerror(errno));
			longjmp(my_jmp_buf, EXIT_FAILURE);
			//exit(EXIT_FAILURE);
		}
	}
	else
	{
#ifdef DEBUG
		static char tmp[1024];
		strncpy(tmp, buf, count);
		tmp[count] = 0;
		printf("I2C send: \"%s\"\n", tmp);
#endif
	}
}

uint8_t linenum_to_cursor(int linenum)
{
	switch (linenum)
	{
		case 1:
			return 0x40;
			break;
		case 2:
			return 0x14;
			break;
		case 3:
			return 0x54;
			break;
		case 0:
		default:
			return 0x0;
			break;
	}
}

#define min(a, b) ((a <= b) ? a : b)

void handle_pipe_input(int i2c_fd, char *buffer, int size)
{
	// FIXME this should handle escape sequences (other than \b, \n) too

	static int line_number = 0;

	// LCD clear
	if (size >= 1 && buffer[0] == '\b')
	{
		uint8_t bytes[] = {0xFE, 0x51};
		i2c_send(i2c_fd, bytes, sizeof(bytes));
		buffer++; size--;
		line_number = 0;
	}

	// Split input into lines
	char *line = strtok(buffer, "\n");
	while (line != NULL)
	{
		// Because of weird cursor positioning on the LCD module, if we write
		// more than 20 characters on line 1 it'll flow over onto line 3, (or
		// off line 2 on to line 4). Thus, we limit lines to 20 chars.

		i2c_send(i2c_fd, line, min(strlen(line), 20));
		line_number = (line_number < 3) ? (line_number + 1) : 0;

		// Go to specified line
		uint8_t bytes[] = {0xFE, 0x45, 0x0};
		bytes[2] = linenum_to_cursor(line_number);
		i2c_send(i2c_fd, bytes, sizeof(bytes));

		// Get next line
		line = strtok(NULL, "\n");
	}
}

int main(int argc, char **argv)
{
	char device[64] = "/dev/i2c-3", fifo_name[64] = "/dev/lcd";
	int addr = LCD_ADDR, addr_sscanf_ret = 1;

	// Setup signal handling before we start
	signal(SIGHUP, signal_handler);
	siginterrupt(SIGHUP, 1);
	signal(SIGQUIT, signal_handler);
	siginterrupt(SIGQUIT, 1);
	signal(SIGTERM, signal_handler);
	siginterrupt(SIGTERM, 1);
	signal(SIGINT, signal_handler);
	siginterrupt(SIGINT, 1);

	int c;
	while( (c = getopt(argc, argv, "a:d:f:n?")) != -1) {
		switch(c){
			case 'a':
				addr_sscanf_ret = sscanf(optarg, "0x%x", &addr);
				break;
			case 'd':
				strncpy(device, optarg, sizeof(device));
				break;
			case 'f':
				strncpy(fifo_name, optarg, sizeof(fifo_name));
				break;
			case 'n':
				daemonize = 0;
				break;
			case '?':
			default:
				usage(argc, argv);
				exit(0);
				break;
		}
	}

	// Setup syslog logging
	if (daemonize == 1)
	{
		setlogmask(LOG_UPTO(LOG_INFO));
		openlog(DAEMON_NAME, LOG_CONS, LOG_USER);
	}
	LOG_MSG("daemon starting up");

	// Check sscanf above worked (delayed until after syslog is initialized)
	if (addr_sscanf_ret != 1)
		LOG_MSG("supplied address invalid, using 0x%02x", addr);

	// Our process ID and Session ID
	pid_t pid, sid;

	if (daemonize)
	{
		// Fork off the parent process
		pid = fork();
		if (pid < 0)
		{
			LOG_MSG("failed to fork daemon process");
			exit(EXIT_FAILURE);
		}

		// If we got a good PID, then we can exit the parent process
		if (pid > 0)
			exit(EXIT_SUCCESS);

		// Change the file mode mask
		umask(0);

		// Create a new SID for the child process
		sid = setsid();
		if (sid < 0)
		{
			LOG_MSG("failed to set daemon process SID");
			exit(EXIT_FAILURE);
		}

		// Change the current working directory
		if ((chdir("/")) < 0)
		{
			LOG_MSG("failed to change daemon working directory");
			exit(EXIT_FAILURE);
		}

		// Close out the standard file descriptors
		close(STDIN_FILENO);
#ifndef DEBUG
		close(STDOUT_FILENO);
#endif
		close(STDERR_FILENO);
	}

	//************************************************************************

	addr >>= 1; // to suit LCD addressing sematics of Newhaven LCD modules

	// Initialise FIFO on which to accept input
	if (mkfifo(fifo_name, S_IRWXU) < 0)
	{
		if (errno != EEXIST)
		{
			LOG_MSG("error creating named pipe for input: %s", strerror(errno));
			exit(EXIT_FAILURE);
		}
		else
		{
			LOG_MSG("warning, named pipe already exists: %s", strerror(errno));
			// FIXME unlink and reopen
		}
	}

	// Connect to I2C LCD and set slave address
	int i2c_fd = open(device, O_RDWR);
	if (i2c_fd < 0)
	{
		LOG_MSG("error opening device: %s", strerror(errno));
		have_lcd = 0;
		goto nolcd;
	}
	if (ioctl(i2c_fd, I2C_SLAVE, addr) < 0) 
	{
		LOG_MSG("error setting I2C slave address: %s", strerror(errno));
		close(i2c_fd);
		have_lcd = 0;
	}

nolcd:
	if (setjmp(my_jmp_buf) == EXIT_FAILURE)
		goto exit1;

	if (!have_lcd)
	{
		LOG_MSG("could not open I2C LCD, printing to stdout for debugging");
	}
	else
	{
		LOG_MSG("using Nehaven LCD attached to %s at real address 0x%x", device, addr);

		// Initialise LCD
		uint8_t bytes[] = {0xFE, 0x41};		// LCD on
		i2c_send(i2c_fd, bytes, sizeof(bytes));
		bytes[1] = 0x51;					// LCD clear
		i2c_send(i2c_fd, bytes, sizeof(bytes));

		uint8_t *string = (uint8_t*)"  Initialising...";
		i2c_send(i2c_fd, string, strlen((char*)string));
	}

	char buffer[256];
	int fifo_fd = -1;
	struct pollfd fds = { .events = POLLIN };

	// Main Loop: read on named pipe, output on LCD
	while (running)
	{
		if (fifo_fd == -1)
		{
#ifdef DEBUG
			LOG_MSG("waiting for connection on %s...", fifo_name);
#endif
			fifo_fd = open(fifo_name, O_RDONLY); // this will block until other end of pipe is opened
			if (fifo_fd < 0)
			{
				if (errno == EINTR)
					continue;
				LOG_MSG("error opening named pipe for input: %s", strerror(errno));
				goto exit3;
			}
			fds.fd = fifo_fd;
#ifdef DEBUG
			LOG_MSG("connected, accepting input on %s", fifo_name);
#endif
		}

		// Wait for input on the named pipe
		int poll_ret = poll(&fds, 1, 1000);
		if (poll_ret == 1 && fds.revents | POLLIN)
		{
			memset(&buffer, 0, sizeof(buffer));
			int bytes_read = read(fifo_fd, &buffer, sizeof(buffer));

			// Write input to LCD
			if (bytes_read > 0)
				handle_pipe_input(i2c_fd, buffer, bytes_read);
			else if (bytes_read == 0)
			{
#ifdef DEBUG
				LOG_MSG("named pipe closed");
#endif
				close(fifo_fd);
				fifo_fd = -1;
			}
			else
			{
				LOG_MSG("error reading from named pipe: %s", strerror(errno));
				goto exit1;
			}
		}
		else if (poll_ret < 0 && running)
		{
			LOG_MSG("error waiting for input on named pipe: %s", strerror(errno));
			goto exit1;
		}
		else
		{
			// Timeout in poll(), no need to worry (timeout is only enabled as insurance anyway)
		}
	}

	LOG_MSG("daemon exiting cleanly");

	close(fifo_fd);
	unlink(fifo_name);
	if (have_lcd)
		close(i2c_fd);

	return 0;

	// Cleanup on error
exit1:
	if (have_lcd)
		close(i2c_fd);
exit2:
	close(fifo_fd);
exit3:
	unlink(fifo_name);
	LOG_MSG("daemon exiting with failure");
	exit(EXIT_FAILURE);
}

