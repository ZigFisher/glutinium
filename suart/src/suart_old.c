/*
 *  SUART -- Socket to UART tunnel utility
 *
 *  Copyright (c) 2011 Volkov Andrey (espirit), Botov Anton (=DeaD=)
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *  THE SOFTWARE.
 *****************************************************************************/


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h> /* UNIX standard function definitions */
#include <fcntl.h> /* File control definitions */
#include <errno.h> /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

int main(int argc, char *argv[])
{
    int debug=0;     //<Print debug info> flag
    int show_help=0; //<Show help page and exit> flag

    int port_num=3000; //Socket address
    speed_t baudrate=B115200; //Baudrate for serial port
    char* ttyPort="/dev/ttyS0\0";

    int arg=1;
    while(arg<argc){
      if(argv[arg][0]!='-'){ printf("error 0 in key %d - bad first symbol in '%s' key\n\n",arg,argv[arg]); show_help=1; };
      if(argv[arg][1]=='?')show_help=1;
      if(argv[arg][1]=='p' || argv[arg][1]=='b'){
        if(argv[arg][2]=='\0'){
          printf("error 1 in -p|b keys\n\n");
          show_help=1;
        }else{
          int val=atoi(argv[arg]+2);
          if(val==0){
            printf("error 2 in -p|b keys\n\n");
            show_help=1;
          }else{
            if(argv[arg][1]=='p'){
              port_num=val;
            }else{
              if(val==9600){ baudrate=B9600; }
              else if(val==19200){ baudrate=B19200; }
              else if(val==38400){ baudrate=B38400; }
              else if(val==57600){ baudrate=B57600; }
              else if(val==115200){ baudrate=B115200; }
              else{ printf("error 3 in -b key\n\n"); show_help=1; };
            };
          };
        }
      };
      if(argv[arg][1]=='s'){
        if(argv[arg][2]=='\0'){ printf("error 4 in -s key\n\n"); show_help=1; };
        ttyPort=(char *)(argv[arg]+2);
      };
      if(argv[arg][1]=='d')debug=1;
      arg++;
    };

    if(show_help==1){
      printf("Suart is tool that allow you to tunnel a local serial connection over a network\n");
      printf("\n");
      printf("Usage: suart [-s<serial_port>] [-p<socket_addr>] [-b<baudrate>] [-d] [-?]\n");
      printf("\n");
      printf("   -s<serial_port> Set serial port, default is /dev/ttyS0\n");
      printf("   -p<socket_addr> Set socket address, default is 3000\n");
      printf("   -b<baudrate> Set serial port baudrate (9600/19200/38400/57600/115200), default is 115200\n");
      printf("   -d Show debug info - transferred data\n");
      printf("   -? Show this help page\n");
      printf("\n");
      printf("Example: suart -s/dev/ttyS1 -p3005 -b9600 -d\n");
      return 0;
    };

    int sock_fd, listener;
    struct sockaddr_in addr;
    char buf[100];
    int bytes_read;
    int optval;

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 0)
    {
      perror("Can't create  socket");
      return -1;
    }

    optval=1;
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)))
    {
	perror("Can't set SO_REUSEADDR on socket");
	return -1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_num);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    { 
     perror("Can't bind port");
     close(sock_fd);
     return -1;
    }

    listen(listener, 1);

//    printf("Listening port   :%s\n",prt);


int tty_fd; /* File descriptor for the port */
int n;
struct termios options;

tty_fd = open(ttyPort, O_RDWR | O_NOCTTY | O_NONBLOCK);
if (tty_fd == -1) perror("open_port: Unable to open serial port");
else
{
fcntl(tty_fd, F_SETFL, 0);

tcgetattr(tty_fd, &options);
cfmakeraw(&options);

/************************************************************************/
// c_cflag
options.c_cflag |= CS8;
options.c_cflag &= ~PARENB;
options.c_cflag &= ~CSTOPB;
options.c_cflag |= CREAD; // Enable Receiver
options.c_cflag |= CLOCAL; // Ignore modem control lines.

cfsetispeed(&options, baudrate);
cfsetospeed(&options, baudrate);

tcsetattr(tty_fd, TCSAFLUSH, &options);
tcflush(tty_fd, TCIOFLUSH); /**/
}


    while(1)
    {
        sock_fd = accept(listener, NULL, NULL);
        if(sock_fd < 0)
        {
         perror("Can't bind port");
         close(sock_fd);
         return -1;
        }
        printf("Connect accepted\n");
        while(1)
        {

          fd_set rfds;
          struct timeval tv;
          int retval;
          int nfds=tty_fd+1;
          if(sock_fd>tty_fd) nfds=sock_fd+1;

          FD_ZERO(&rfds);
          FD_SET(tty_fd, &rfds);
          FD_SET(sock_fd, &rfds);

          tv.tv_sec = 10;
          tv.tv_usec = 0;
          retval = select(nfds, &rfds, NULL, NULL, &tv);

          if (retval)
          {
            if(debug)printf("Some data available.\n");
            if (FD_ISSET(tty_fd, &rfds))
            {
              bytes_read = read(tty_fd, buf, 50);
              send(sock_fd, buf, bytes_read, 0);
              buf[bytes_read]=0;
              if(debug)printf("=>Socket: %s\n",buf);
              if(!debug)printf("=>Socket: %d bytes\n",bytes_read);
            };
            if (FD_ISSET(sock_fd, &rfds))
            {
              bytes_read = recv(sock_fd, buf, 50, 0);
              if(bytes_read <= 0) break;
              write(tty_fd, buf, bytes_read);
              buf[bytes_read]=0;
              if(debug)printf("Socket=>: %s\n",buf);
              if(!debug)printf("Socket=>: %d bytes\n",bytes_read);
            };
          }
          else
            printf("No data in 10 seconds.\n");

        }

        close(sock_fd);
    }

    return 0;
}
