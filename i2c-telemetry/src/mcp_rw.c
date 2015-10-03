#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <string.h>
// Строки ниже необходимы для rasbian/debian.В Suse должны быть закаментированы.
#include <linux/i2c.h>
#include "smbus.h" 

#define mcp_I2C_ADDRESS 0x20 // адрес устройства в сети i2c,задается перемычками A0,A1,A2 на микросхеме.


// ниже 2 строки задают направление порта ,1 - на ввод,0 - на вывод.
#define mcp_bank_a 0b00000100  //байты 7 по 0 порт
#define mcp_bank_b 0b00000000 //байты 8 по 15 порт
// следущие 2 строки-подтяжка резистором 100 кОм на +, 1 - включен.Актуально для режима на ввод.
#define gppu_bank_a 0b00000100 //байты 7 по 0 порт
#define gppu_bank_b 0b00000000 //байты 8 по 15 порт
// регистры 02 03 для инвертирования
#define iopol_bank_a 0b00000110 //байты 7 по 0 порт
#define iopol_bank_b 0b00000000 //байты 8 по 15 порт





int fd;
unsigned int  utv;
int l=1;
int x=0;

// Open a connection to the mcp
// Returns a file id
int mcp_i2c_Begin()
{
 
   char *fileName = "/dev/i2c-7"; // указать Ваш адрес
   
   // Open port for reading and writing
   if ((fd = open(fileName, O_RDWR)) < 0){
     printf("i2c device not found \n");
      exit(1);
   } 
   // Set the port options and set the address of the device
   if (ioctl(fd, I2C_SLAVE, mcp_I2C_ADDRESS) < 0) {               
      close(fd);
      printf("on i2c mcp device not found \n");
      exit(1);
   }

   return fd;
}

// Read a byte to mcp
__s32 mcp_i2c_Read_byte(int fd, __u8 address)
{
   __s32 res = i2c_smbus_read_byte_data(fd, address);
   if (res < 0) {
      printf("error \n");
      close(fd);
      exit(1);
   }

   return res;
}

//Write a byte to mcp
void mcp_i2c_Write_Byte(int fd, __u8 address, __u8 value)
{
   if (i2c_smbus_write_byte_data(fd, address, value) < 0) {
      close(fd);
      exit(1);
   }
}
// чтение состояния порта
int mcp_read (int mcppin)
{
 if (mcppin < 0 || mcppin >15) {
       printf("Please select a valid GPIO pin \n");
        exit(1);
 }

 int fd =  mcp_i2c_Begin();
 int regbank;

if (mcppin < 8){
  
 mcp_i2c_Write_Byte(fd,0x00,mcp_bank_a);
 mcp_i2c_Write_Byte(fd,0x0c,gppu_bank_a);
 
    regbank=0x12;

//printf("bank A\n");
} else {
  
    mcp_i2c_Write_Byte(fd,0x01,mcp_bank_b);
    mcp_i2c_Write_Byte(fd,0x0d,gppu_bank_b);
      
   regbank=0x13;
   mcppin=mcppin-8;

 //  printf("bank B\n");
}

    utv = mcp_i2c_Read_byte(fd,regbank);

      while(x <  mcppin)
    {
      x = x+ 1;
      l=l*2;
}


if (utv & l) return 1;
else return 0;

close(fd);

}


// запрос таблицы статусов портов
void mcp_state_in () {   
      int fd =  mcp_i2c_Begin();
     utv = mcp_i2c_Read_byte(fd,0x12);
 //    printf ("--%d--\n",utv);
             while(x <  8)
    {
      
 if ((mcp_bank_a & l)) {
      if ((utv & l)) printf ("%d 1\n",x);
     else printf ("%d 0\n",x);

} else printf ("%d -\n",x);
      l=l*2;
     x = x+ 1;
     
}

      x=0;
      l=1;

          utv = mcp_i2c_Read_byte(fd,0x13);
//	      printf ("--%d--\n",utv);
             while(x <  8)
    {
       if ((mcp_bank_b & l)) {
      if ((utv & l)) printf ("%d 1\n",x+8);
     else printf ("%d 0\n",x+8);

     } else printf ("%d -\n",x+8);
           l=l*2;
     x = x+ 1;
}
  	      
close(fd);

}
// установка состояния порта
void mcp_write (int mcppin,int mcpst)
{
 if (mcppin < 0 || mcppin >15) {
       printf("Please select a valid GPIO pin \n");
        exit(1);
 }
   if (mcpst != 0 && mcpst != 1) {
      printf("Не верно задан статус порта 0 или 1\n");
        exit(1);
 }
  
 int fd =  mcp_i2c_Begin();
 int regbank;

if (mcppin < 8){
  
 mcp_i2c_Write_Byte(fd,0x00,mcp_bank_a);
 mcp_i2c_Write_Byte(fd,0x0c,gppu_bank_a);
 
    regbank=0x14;

//printf("bank A\n");
} else {
  
    mcp_i2c_Write_Byte(fd,0x01,mcp_bank_b);
    mcp_i2c_Write_Byte(fd,0x0d,gppu_bank_b);
      
   regbank=0x15;
   mcppin=mcppin-8;

 //  printf("bank B\n");
}

    utv = mcp_i2c_Read_byte(fd,regbank);

      while(x <  mcppin)
    {
      x = x+ 1;
      l=l*2;
}


if (!(utv & l) && mcpst==1) mcp_i2c_Write_Byte(fd,regbank,utv+l);
if (utv & l && mcpst==0) mcp_i2c_Write_Byte(fd,regbank,utv-l);

close(fd);

}


// запрос таблицы статусов портов
void mcp_state_out () {   
      int fd =  mcp_i2c_Begin();
     utv = mcp_i2c_Read_byte(fd,0x14);
 //    printf ("--%d--\n",utv);
             while(x <  8)
    {
      
 if (!(mcp_bank_a & l)) {
      if ((utv & l)) printf ("%d 1\n",x);
     else printf ("%d 0\n",x);

} else printf ("%d -\n",x);
      l=l*2;
     x = x+ 1;
     
}

      x=0;
      l=1;

          utv = mcp_i2c_Read_byte(fd,0x15);
//	      printf ("--%d--\n",utv);
             while(x <  8)
    {
       if (!(mcp_bank_b & l)) {
      if ((utv & l)) printf ("%d 1\n",x+8);
     else printf ("%d 0\n",x+8);

     } else printf ("%d -\n",x+8);
           l=l*2;
     x = x+ 1;
}
  	      
close(fd);

}
// основное тело программы

int main(int argc, char **argv)

{
   if (argc == 1) {
     printf("%s <команда>\nin -чтение данных из порта,установленых на ввод\nout-установка состояния порта,установленного на вывод.\n", argv[0]);
     return 1;
   } 
   else if (argc == 2) {
  if(strcmp(argv[1], "out") == 0) mcp_state_out();
  else if(strcmp(argv[1], "in") == 0) mcp_state_in();
  else printf("Нет такой команды\n");
  return 1;
   }
   
   if(strcmp(argv[1], "out") == 0) {
   
        if (argc != 4) {
             printf("Настройка порта: %s <порт> <состояние>\n", argv[0]);

return 1;
}
int mcppin = atoi(argv[2]);
int mcpst = atoi(argv[3]);

mcp_write (mcppin,mcpst);
     
     
   }
   else if(strcmp(argv[1], "in")== 0) {
   if (argc != 3) {
             printf("Чтение одного порта: %s <порт>\n", argv[0]);
     mcp_state_in();
     return 1;
   }
   int mcppin = atoi(argv[2]);

   printf ("%d\n",mcp_read (mcppin));
   
   }





   return 0;
}