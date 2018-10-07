
/* Idea and basic c++ source file get from https://habr.com/post/397637/ (Russian) */

#include <stdio.h>   /* Стандартные объявления ввода/вывода */
#include <string>
#include <iostream>
#include <cstring>

using namespace std;
#include <unistd.h>  /* Объявления стандартных функций UNIX */
#include <fcntl.h>   /* Объявления управления файлами */
#include <errno.h>   /* Объявления кодов ошибок */
#include <termios.h> /* Объявления управления POSIX-терминалом */
#include <sys/types.h>
#include <sys/stat.h>

int fd; /* Файловый дескриптор для порта */
char buf[512];/*размер зависит от размера строки принимаемых данных*/
int main (int argc, char* argv[])
 {
    int iIn,iOut;
    string UPSAnswer;
    if (argc>=2)
{
        fd = open(argv[1], O_RDWR | O_NOCTTY | O_NDELAY); /*'open_port()' - Открывает последовательный порт */
        if (fd == -1)   {
           printf("error port\n");
           perror("open_port: Unable to open port - ");   }
     else
        {
         struct termios options; /*структура для установки порта*/
         tcgetattr(fd, &options); /*читает пораметры порта*/

         cfsetispeed(&options, B2400); /*установка скорости порта*/
//         cfsetospeed(&options, B2400); /*установка скорости порта*/

         options.c_cflag &= ~PARENB; /*выкл проверка четности*/
         options.c_cflag &= ~CSTOPB; /*выкл 2-х стобит, вкл 1 стопбит*/
         options.c_cflag &= ~CSIZE; /*выкл битовой маски*/
         options.c_cflag |= CS8; /*вкл 8бит*/
         tcsetattr(fd, TCSANOW, &options); /*сохронения параметров порта*/
        }

        char Params[64];

        if(argc>=3)
        {
            if (!strcmp(argv[2],"status"))
            {
                iOut = write(fd, "Q1\r", 3);
                usleep(800000);
                if (iOut < 0)  fputs("write() of 4 bytes failed!\n", stderr);
                iIn=read(fd,buf,250); /*чтения приходящих данных из порта*/

                strncpy(Params,&buf[38],11);
                Params[46]=0;
            }
            else if (!strcmp(argv[2],"help"))
            {
                printf ("help\r\n");
//              printf ("InVolt FaultVolt OutVolt Current Freq UBatt UTemp NA\r\n");
            }
            else if (!strcmp(argv[2],"name"))
            {
                iOut = write(fd, "I\r", 3);
                usleep(800000);
                if (iOut < 0)  fputs("write() of 4 bytes failed!\n", stderr);
                iIn=read(fd,buf,250); /*чтения приходящих данных из порта*/
                strncpy(Params,&buf[0],60);

            }
            else if (!strcmp(argv[2],"stat2"))
            {
                iOut = write(fd, "F\r", 3);
                usleep(800000);
                if (iOut < 0)  fputs("write() of 4 bytes failed!\n", stderr);
                iIn=read(fd,buf,250); /*чтения приходящих данных из порта*/
                strncpy(Params,&buf[1],60);
                Params[20]=0;
            }
            else
            {
                printf ("WTF ?\r\n");
            }
        }
        else
        {
           /* читаем данный по Q1 */
                usleep(1800);
                iIn=read(fd,buf,250);
                usleep(1800);
           iOut = write(fd, "Q1\r", 3);
           usleep(800000);
           if (iOut < 0)  fputs("write() of 4 bytes failed!\n", stderr);
           iIn=read(fd,buf,250); /*чтения приходящих данных из порта*/

           strncpy(Params,&buf[1],36);
           Params[36]=' ';
        }
    close(fd);
    printf("%s\r\n",Params);
}
else
printf("Usage %s /dev/ttySx", argv[0]);

}
