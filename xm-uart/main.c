#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <termios.h>
#include <unistd.h>
#include <getopt.h>

const int addressPTZ = 1;

// #define AUTO_FOCUS

#if defined(AUTO_FOCUS)
#define SYNC 0xc5
#else
#define SYNC 0xff
#endif

const uint8_t init[] = {0xa5, 0x7b, 0x9e, 0xf0, 0xef, 0xee, 0xe0, 0xf4};

static void send_cmd(int fd, int panSpeed, int tiltSpeed, int zoomSpeed) {
  uint8_t command1 = 0, command2 = 0, data1 = 0, data2 = 0, checkSum = 0;

  if (panSpeed < 0) {
    command2 |= 0x04;
    panSpeed *= (-1);
  } else if (panSpeed > 0) {
    command2 |= 0x02;
  }
  data1 = (uint8_t)panSpeed * 63 / 100;

  if (tiltSpeed < 0) {
    command2 |= 0x10;
    tiltSpeed *= (-1);
  } else if (tiltSpeed > 0) {
    command2 |= 0x08;
  }
  data2 = (uint8_t)tiltSpeed * 63 / 100;

  if (zoomSpeed < 0) {
    command2 |= 0x40;
  } else if (zoomSpeed > 0) {
    command2 |= 0x20;
  }

  checkSum = (addressPTZ + command1 + command2 + data1 + data2) % 100;

  uint8_t bstr[] = {SYNC,  addressPTZ, command1, command2,
                    data1, data2,      checkSum, 0x5c};
  write(fd, bstr, sizeof(bstr));
}

static void set_focus(int fd, bool near) {
  uint8_t command1 = 0, command2 = 0, data1 = 0, data2 = 0, checkSum = 0;
  checkSum = (addressPTZ + command1 + command2 + data1 + data2) % 100;

  if (near)
    command1 = 1;
  else
    command2 = 0x80;

  uint8_t bstr[] = {SYNC,  addressPTZ, command1, command2,
                    data1, data2,      checkSum, 0x5c};
  write(fd, bstr, sizeof(bstr));
}

static void init_ptz(int fd) { write(fd, init, 8); }

#define CMD(name, panSpeed, tiltSpeed, zoomSpeed)                              \
  puts(name);                                                                  \
  send_cmd(uart, panSpeed, tiltSpeed, zoomSpeed);                              \
  break;

static void dump_hex(const void *data, size_t size) {
  char ascii[17];
  size_t i, j;
  ascii[16] = '\0';
  for (i = 0; i < size; ++i) {
    printf("%02X ", ((uint8_t *)data)[i]);
    if (((uint8_t *)data)[i] >= ' ' && ((uint8_t *)data)[i] <= '~') {
      ascii[i % 16] = ((uint8_t *)data)[i];
    } else {
      ascii[i % 16] = '.';
    }
    if ((i + 1) % 8 == 0 || i + 1 == size) {
      printf(" ");
      if ((i + 1) % 16 == 0) {
        printf("|  %s \n", ascii);
      } else if (i + 1 == size) {
        ascii[(i + 1) % 16] = '\0';
        if ((i + 1) % 16 <= 8) {
          printf(" ");
        }
        for (j = (i + 1) % 16; j < 16; ++j) {
          printf("   ");
        }
        printf("|  %s \n", ascii);
      }
    }
  }
}

static void parse_incoming(const char *data, size_t size) {
  if ((size > 0) && (data[0] == 'X')) {
    char buf[20] = {0};
    memcpy(buf, data, MIN(sizeof(buf) - 1, size));
    printf("Magnification: %s\n", buf);
    return;
  }

  if ((size == 5) && !memcmp(data, "\xEF\x01\x02\x01", 4)) {
    if (data[4] == 1) {
      printf("NIGHT mode\n");
      return;
    } else if (data[4] == 0) {
      printf("DAY mode\n");
      return;
    }
  }

  dump_hex(data, size);
}

int main(int argc, char *argv[]) {
  struct termios ctrl;
  tcgetattr(STDIN_FILENO, &ctrl);
  ctrl.c_lflag &= ~ICANON; // turning off canonical mode makes input unbuffered
  ctrl.c_lflag &= ~ECHO;   // disable echo
  tcsetattr(STDIN_FILENO, TCSANOW, &ctrl);
  int c;
  char *device = "/dev/ttyAMA0";
  
  while ((c = getopt(argc, argv, "d:")) != -1) {
    switch (c) {
      case 'd':
        device = optarg;
        break;
      default:
        printf("Invalid Argument %c\n", c);
        printf("Usage : %s\n");       
        printf("\t -d tty device, default /dev/ttyAMA0\n\n");
        printf("Commands:\n+ - (Zoom) z x (Focus) h j k l (Pan Tilt) Space (Cancel)\n");
        return (-1);
    }
  }

  int uart = open(device, O_RDWR | O_NOCTTY);
  if (uart == -1) {
    printf("Error no is : %d\n", errno);
    printf("Error description is : %s\n", strerror(errno));
    return (-1);
  };

  struct termios options;
  tcgetattr(uart, &options);
  cfsetspeed(&options, B9600);

  options.c_cflag &= ~CSIZE;  // Mask the character size bits
  options.c_cflag |= CS8;     // 8 bit data
  options.c_cflag &= ~PARENB; // set parity to no
  options.c_cflag &= ~PARODD; // set parity to no
  options.c_cflag &= ~CSTOPB; // set one stop bit

  options.c_cflag |= (CLOCAL | CREAD);

  options.c_oflag &= ~OPOST;

  options.c_lflag &= 0;
  options.c_iflag &= 0; // disable software flow controll
  options.c_oflag &= 0;

  cfmakeraw(&options);
  tcsetattr(uart, TCSANOW, &options);

  init_ptz(uart);
  int flags = fcntl(uart, F_GETFL, 0);
  fcntl(uart, F_SETFL, flags | O_NONBLOCK);

  printf("Xiongmai UART Motors, get in a car and fasten your safety belt\n");
  printf(
      "Commands:\n+ - (Zoom) z x (Focus) h j k l (Pan Tilt) Space (Cancel)\n");

  while (1) {

    struct pollfd pfds[2] = {
        {.fd = 0, .events = POLLIN},
        {.fd = uart, .events = POLLIN},
    };

    poll(pfds, 2, -1);

    if (pfds[0].revents & POLLIN) {
      char ch;
      int i = read(0, &ch, 1);

      if (!i) {

        printf("stdin closed\n");
        return 0;
      }
      switch (ch) {
      case '-':
        CMD("Zoom out", 0, 0, -1);

      case '+':
        CMD("Zoom in", 0, 0, 1);

      case '\n':
      case ' ':
        CMD("Cancel", 0, 0, 0);

      case 'z':
        puts("Focus near");
        set_focus(uart, true);
        break;

      case 'x':
        puts("Focus far");
        set_focus(uart, false);
        break;

        // top
      case 'h':
        CMD("Pan left", 100, 0, 0);

      case 'i':
      case 'l':
        CMD("Pan right", -100, 0, 0);

      case 'n':
      case 'j':
        CMD("Tilt down", 0, -100, 0);

      case 'e':
      case 'k':
        CMD("Tilt up", 0, 100, 0);

      default:
        printf("Unknown command %c\n", ch);
      }
    }

    if (pfds[1].revents & POLLIN) {
      uint8_t rbuf[1024];

      int i = read(uart, rbuf, sizeof(rbuf));

      if (!i) {
        printf("UART closed, make sure system getty is disabled on UART\n");
        return 0;
      }

      if (i != -1)
        parse_incoming(rbuf, i);
    }
  }
  send_cmd(uart, 0, 0, 0);
  close(uart);
}
