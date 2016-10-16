#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

#include <curl/curl.h>
#include <stdio.h>


void set_blocking (int fd, int should_block)
{
		struct termios tty;
		memset (&tty, 0, sizeof(tty));
		if (tcgetattr (fd, &tty) !=0)
		{
				printf ("error %d from tggetattr", errno);
				return;
		}

		tty.c_cc[VMIN]	= 	should_block ? 1 : 0;
		tty.c_cc[VTIME]	= 5;
		if (tcsetattr (fd, TCSANOW, &tty) != 0){
			printf ("error %d setting term attributes", errno);
		}
}

int
set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf ("error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                printf ("error %d from tcsetattr", errno);
                return -1;
        }
        return 0;
}


 int open_port(void)
 {
        int fd;
  char *portname = "/dev/ttyUSB0" ;
fd = open (portname, O_RDWR | O_NOCTTY | O_SYNC);
if (fd < 0)
{
        printf  ("error %d opening %s: %s", errno, portname, strerror (errno));
        return;
}

set_interface_attribs (fd, B9600, 0);  // set speed to 115,200 bps, 8n1 (no parity)
set_blocking (fd, 0);

   return (fd);
 }

int setaddr (void)
{

 unsigned char cmd[7] = {0xB4,0xC0,0xA8,0x01,0x02,0x00,0x1F}; // message to send
int setaddr;
int mainfd=0;
unsigned char byte;
int cont= 0;

unsigned  char result[7];

mainfd = open_port();


setaddr = write(mainfd, cmd, sizeof(cmd));

sleep(1);

while(cont<7){
                                setaddr= read(mainfd, &byte, sizeof(byte));
                                if (setaddr < 0 ) {
                                        close(mainfd);

                                return -2;
                                        }
                                else
                                                {
                                                result[cont]=byte;
                               // printf("result[%d]=%d %02x \n",cont,result[cont],result[cont]);
                                                }

                                cont++;

}
close(mainfd);

//sprintf(volt,"%d.%d",((result[1]<<8))+result[2],result[3]);

return 0;

}

int getvoltage (char *volt)
{

 unsigned char cmd[7] = {0xB0,0xC0,0xA8,0x01,0x02,0x00,0x1B}; // message to send
int getvolt;
int mainfd=0;
unsigned char byte;
int cont= 0;

unsigned  char result[7];

mainfd = open_port();

getvolt = write(mainfd, cmd, sizeof(cmd));

while(cont<7){
                                getvolt= read(mainfd, &byte, sizeof(byte));
                                if (getvolt < 0 ) {
                                        close(mainfd);

                                        return -2;
                                                  }
                                else
                                                {
                                                result[cont]=byte;

                                                }

                                cont++;

}

sprintf(volt,"%d.%d",((result[1]<<8))+result[2],result[3]);


close(mainfd);


return 0;


}

int getcurrent (char *ampere)
{

unsigned char cmd[7] = {0xB1,0xC0,0xA8,0x01,0x02,0x00,0x1C}; // message to send
int getamp;
int mainfd=0;
unsigned char byte;
int cont= 0;
unsigned  char result[7];

mainfd = open_port();

getamp = write(mainfd, cmd, sizeof(cmd));

while(cont<7){
                                getamp= read(mainfd, &byte, sizeof(byte));
                                if (getamp < 0 ) {
                                        close(mainfd);

                                return -2;
                                        }
                                else
                                                {
                                                result[cont]=byte;

                                                }
                                cont++;
}

sprintf(ampere,"%d.%d",((result[1]<<8))+result[2],result[3]);

close(mainfd);
return 0;

}


int getpower (char *watt)
{

unsigned char cmd[7] = {0xB2,0xC0,0xA8,0x01,0x02,0x00,0x1D}; // message to send
int getpower;
int mainfd=0;
unsigned char byte;
int cont= 0;

unsigned  char result[7];

mainfd = open_port();

getpower = write(mainfd, cmd, sizeof(cmd));

while(cont<7){
                                getpower= read(mainfd, &byte, sizeof(byte));
                                if (getpower < 0 ) {
                                        close(mainfd);

                                return -2;
                                        }
                                else
                                                {
                                                result[cont]=byte;

                                                }

                                cont++;

}

sprintf(watt,"%d.%d",((result[1]<<8))+result[2],result[3]);
close(mainfd);
return 0;

}

int getenergy (char *joule)
{

 unsigned char cmd[7] = {0xB3,0xC0,0xA8,0x01,0x02,0x00,0x1E}; // message to send
int getenergy;
int mainfd=0;
unsigned char byte;
int cont= 0;

unsigned  char result[7];

mainfd = open_port();

getenergy = write(mainfd, cmd, sizeof(cmd));

while(cont<7){
                                getenergy= read(mainfd, &byte, sizeof(byte));
                                if (getenergy < 0 ) {
                                        close(mainfd);

                                return -2;
                                        }
                                else
                                                {

                                                result[cont]=byte;

                                                }
                                cont++;
}

sprintf(joule,"%u",((result[1])<<16)+((result[2])<<8)+result[3]);

close(mainfd);
return 0;


}


int main(int argc, char **argv)
{

char volt[7]="";
char ampere[7]="";
char watt[7]="";
char joule[7]="";
int cont =0;
 CURL *curl;
  CURLcode res;

int res0=0,res1=0, res2=0, res3=0, res4=0, mainfd=0;


  // configure influxdb connection
  curl = curl_easy_init();
  if (curl) {
    char url[256];
    snprintf(url, sizeof(url), "http://localhost:8086/write?db=mydb");
    curl_easy_setopt(curl, CURLOPT_URL, url);
    char auth[256];
    snprintf(auth, sizeof(auth), "admin:admin" );
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(curl, CURLOPT_USERPWD, auth);
  } else {
    fprintf(stderr, "could not initialize curl\n");
    return 1;
  }


while(1)
        {

        res1=getvoltage(volt);
        sleep(1);
        res2=getcurrent(ampere);
        sleep(1);
        res3=getpower(watt);
        sleep(1);
        res4=getenergy(joule);
        sleep(1);

if (curl) {
        CURLcode res = CURLE_OK; // curl response
        char post[128*4] = {0}; // string to submit data to DB using POST request
        snprintf(post, sizeof(post), "voltage,meter=%s value=%s\ncurrent,meter=%s value=%s\npower,meter=%s value=%s\nenergy,meter=%s value=%s\n", "001", volt,"001", ampere, "001", watt, "001", joule);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post);
        res = curl_easy_perform(curl);
        if (res!= CURLE_OK) {
          fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
      }
        /*char url[256]="";
        sprintf(url,"wget -qO- \"http://tplserver.tplinfo.com/energy/insert.php?v=%s&a=%s&w=%s&id=%s&u=tpl&p=iot\"",volt,ampere,watt,"001");
        system(url);
*/
        printf("volt=%s ampere=%s watt=%s joule=%s\n", volt, ampere, watt, joule);

        }

}
