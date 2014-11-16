#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include "smbus.h"
#include <ncurses.h> /* sudo apt-get install libncurses5-dev */

// gcc -o pcf8591_2 -lncurses pcf8591_2.c

int main( int argc, char **argv )
{
   int i;
   int r;
   int fd;
   int aout;
   unsigned char command[2];
   unsigned char value[4];
   unsigned char str[8];
   useconds_t delay = 5000;

   char *dev = "/dev/i2c-0";
   int addr = 0x48;

   int j;
   int key;

   initscr();
   noecho();
   cbreak();
   nodelay(stdscr, true);
   curs_set(0);
   printw("PCF8591");

   mvaddstr(10, 0, "Brightness");
   mvaddstr(12, 0, "Temperature");
   mvaddstr(14, 0, "?");
   mvaddstr(16, 0, "Resistor");
   refresh();
   fd = open(dev, O_RDWR );
   if(fd < 0)
   {
      perror("Opening i2c device node\n");
      return 1;
   }

   r = ioctl(fd, I2C_SLAVE, addr);
   if(r < 0)
   {
      perror("Selecting i2c device\n");
   }

   command[1] = 0;
   aout = 0;
   while(1)
   {
      for(i = 0; i < 4; i++)
      {
         command[1]=aout++;
         command[0] = 0x40 | ((i + 1) & 0x03); // output enable | read input i
         r = write(fd, &command, 2);
         usleep(delay);
         // the read is always one step behind the selected input
         r = read(fd, &value[i], 1);
         if(r != 1)
         {
            perror("reading i2c device\n");
         }
         usleep(delay);

         sprintf(str, "%3d", value[i]);
         mvaddstr(10+i+i, 12, str);
         value[i] = value[i] / 4;
         move(10 + i + i, 16);

         for(j = 0; j < 64; j++)
         {
            if(j < value[i])
            {
               addch('*');
            }
            else
            {
               addch(' ');
            }
         }
      }
      refresh();

      key = getch();
      if(key == 43)
      {
         command[1]++;
      }
      else if(key == 45)
      {
         command[1]--;
      }
      else if(key > -1)
      {
         break;
      }
   }

   endwin();
   close(fd);
   printf("%d\n", key);
   return(0);
}