/* Name: nRF-USB 1.0
 * Страница проекта http://homes-smart.ru/index.php/oborudovanie/bez-provodov-2-4-ggts/50-nrf24l01-usb
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <usb.h>        /* this is libusb */
#include "opendevice.h" /* common code moved to separate module */



#include "usbconfig.h"

  
#define BIN(x)  ((((x) &   1) <<  0) | \
                 (((x) &   2) <<  3) | \
                 (((x) &   4) <<  6) | \
                 (((x) &   8) <<  9) | \
                 (((x) &  16) << 12) | \
                 (((x) &  32) << 15) | \
                 (((x) &  64) << 18) | \
                 (((x) & 128) << 21))

#define byted 0
#define intd 1
#define uintd 2
#define longd 3
#define ulongd 4
#define floatd 5
#define end 255

usb_dev_handle      *handle = NULL;
char                buffer[32];
int                 cnt;

const char* speeddata[]= {"250kBit/s","1MBit/s","2MBit/s"};

static void usage(char *name)
{
    fprintf(stderr, "Пример работы с сайта http://homes-smart.ru/\nВерсия nRF-USB 1.0 от 04.01.14\nusage:\n");
    fprintf(stderr, "  %s read <номер клиента>.............Прочитать данные с радиомодуля.\n", name);
    fprintf(stderr, "  %s write <номер клиента> <data>.....Отправить данные на радиомодуль.\n", name);
    fprintf(stderr, "  %s status ..........................Некоторые данные.\n", name);
    fprintf(stderr, "  %s setupch <номер канала>...........Установка канала.\n", name);
    fprintf(stderr, "  %s setupcount <количество клиентов>..Установка количества клиентов.\n", name);
    fprintf(stderr, "  %s setupspeed <скорость>............Установка скорости передачи.\n", name);
    fprintf(stderr, "  %s setupautoack <статус>............Установка потверждение приема.\n", name);    
    
#if ENABLE_TEST
    fprintf(stderr, "  %s test ..... run driver reliability test\n", name);
#endif /* ENABLE_TEST */
}

// самодельщина костыли:----------------------------------------

int num=0;
unsigned char byte1 (){
 char f=buffer[num];
  num++;
  return f;
}
int byte2 (){
int f=buffer[num];
  num++;
  f=buffer[num]*256+f;
  num++;
return f;
}

long byte4 (){
 unsigned  long  f=(buffer[num]+buffer[num+1]*256+buffer[num+2]*65536+buffer[num+3]*65536*256);
  num=num+4;
return f; 
}

float bytefloat (){
float f;
 char unsigned arr[4] = {buffer[num],buffer[num+1],buffer[num+2],buffer[num+3]};
 memcpy(&f,arr,sizeof f);
 num=num+4;
  
 return f; 
}
// ----------------------------------------------------------
void dbg (){ // вывод сырых данных массива
	int i;
       
        for(i = 0; i < 19; i++) printf("%u ,",(unsigned  int)buffer[i]);
	printf("\n");
}

#include "usbnrf.h"
//-----------------------------------------------------------

void readdata (int len,int chanel){ // char(1) / 2 / 2 / char(1) / 2/ 4 / float(4) 

num=0;
chanel--;
int c=0;

for (num=0;num<len;){
  
    if (dtstr[chanel][c]==byted)	printf("%d",byte1());
else if (dtstr[chanel][c]==intd)	printf("%d",byte2());
else if (dtstr[chanel][c]==uintd)	printf("%hu",(unsigned int)byte2());
else if (dtstr[chanel][c]==longd)	printf("%d",byte4());
else if (dtstr[chanel][c]==ulongd)	printf("%hu",(unsigned long)byte4());
else if (dtstr[chanel][c]==floatd)	printf("%0.1f",bytefloat());
else if (dtstr[chanel][c]==end) {
 printf("\n");
 return;
}
  printf(";"); // разделитель
c++;
}
printf("\nНет параметра end\n");
} 

//-----------------------------------------------------------
int main(int argc, char **argv)
{
//usb_dev_handle      *handle = NULL;
const unsigned char rawVid[2] = {USB_CFG_VENDOR_ID}, rawPid[2] = {USB_CFG_DEVICE_ID};
char                vendor[] = {USB_CFG_VENDOR_NAME, 0}, product[] = {USB_CFG_DEVICE_NAME, 0};
//char                buffer[8];

int                  vid, pid;

    usb_init();
    if(argc < 2){   /* we need at least one argument */
        usage(argv[0]);
        exit(1);
    }
    /* compute VID/PID from usbconfig.h so that there is a central source of information */
    vid = rawVid[1] * 256 + rawVid[0];
    pid = rawPid[1] * 256 + rawPid[0];
    /* The following function is in opendevice.c: */
    if(usbOpenDevice(&handle, vid, vendor, pid, product, NULL, NULL, NULL) != 0){
        fprintf(stderr, "Could not find USB device \"%s\" with vid=0x%x pid=0x%x\n", product, vid, pid);
        exit(1);
    }

if(strcasecmp(argv[1], "status") == 0){

               cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 114, 0, 0, buffer, sizeof(buffer), 5000);
        if(cnt < 1){
            if(cnt < 0){
                fprintf(stderr, "USB error: %s\n", usb_strerror());
            }else{
                fprintf(stderr, "only %d bytes received.\n", cnt);
            }
        }else{ 

	int mem=buffer[0]+(buffer[1]*256);
	printf("Версия USB устройства %0.1f.\n",(float)buffer[4]/10);
	if (mem<0) printf("Не хватает ОЗУ %hu байт.\n",65536-mem);
	else printf("Свободно ОЗУ %hu байт.\n",mem);
	printf("Длинна пакета %hu байт.\nЗанято массивом данных %hu байт.\n",buffer[2],(buffer[2]+5)*buffer[3]);
	}
}else if(strcasecmp(argv[1], "read") == 0){ // чтение данных

       if(argc > 2) {
	
	   int ch= atoi(argv[2]);
        
	  
               cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, ch, 0, 0, buffer, sizeof(buffer), 5000);
        if(cnt < 1){
            if(cnt < 0){
                fprintf(stderr, "USB error: %s\n", usb_strerror());
            }else{
                fprintf(stderr, "only %d bytes received.\n", cnt);
            }
        }else{  

if (buffer[0]==-1)  printf("Неверный канал.\n");

else readdata (cnt,ch);

        }
       
        } else  printf("Не указаны обязательные параметры.\n");
        
             }else if(strcasecmp(argv[1], "write") == 0){ // запись и чтение данных
       if(argc > 3) {
	
	   int ch= atoi(argv[2]);
	   
	   int data[4];
	   data[3]=data[2]=data[1]=data[0]=0;
	
	   data[0]= atoi(argv[3]);
        
      if(argc > 4) data[1]= atoi(argv[4]);
      if(argc > 5) data[2]= atoi(argv[5]);
      if(argc > 6) data[3]= atoi(argv[6]);
	   
       
             cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, ch+50, data[0]+data[1]*256, data[2]+data[3]*256, buffer, sizeof(buffer), 5000);

        if(cnt < 1){
            if(cnt < 0){
                fprintf(stderr, "USB error: %s\n", usb_strerror());
            }else{
                fprintf(stderr, "only %d bytes received.\n", cnt);
            }
        }else{  

	if (buffer[0]==-1)  printf("Неверный канал.\n");
	else printf("OK\n");
	}
        
        
         
        } else  printf("Не указаны обязательные параметры.\n");


    }else if(strcasecmp(argv[1], "setupch") == 0){ // -----------установка канала
      int ch;
      
             if(argc > 2) {
	       ch= atoi(argv[2]); // задать канал
	if ((ch>=0 && ch<126) || (ch>=2400 && ch<2526) ){ // канал норм
	  
	  if (ch>=2400) ch-=2400;
	  
	    cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 110, ch, 0, buffer, sizeof(buffer), 5000);
	
	    printf("Вы установлили канал %d,частота %d Мгц.\n",ch,ch+2400);
	    
	    } else printf("Не верный диапозон каналов.Возможные каналы 0..125. Частоты: 2400..2515 Мгц.\n");
	    
	     } else { // показать канал
	       
	       cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 110, 255, 0, buffer, sizeof(buffer), 5000);
	        if (buffer[0]==255) printf("Установлен канал по умолчанию: 25,частота 2425\n");
	        else printf("Установлен канал: %d ,частота %d Мгц.\n",buffer[0],buffer[0]+2400);
	     }
	     
	     
	        }else if(strcasecmp(argv[1], "setupautoack") == 0){ // -----------
		  
	             if(argc > 2) {
	    int aa;
	       aa= atoi(argv[2]); // задать потверждение
	
	    cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 117, aa, 0, buffer, sizeof(buffer), 5000);
	
	    if (aa==0) printf("потверждение выключено\n");
	    else printf("потверждение включено\n");
	    
	     } else { // показать потверждение
	       
	       cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 117, 255, 0, buffer, sizeof(buffer), 5000);
	        if (buffer[0]==255) printf("По умолчанию потверждение: включено\n");
	        else {

		if (buffer[0]==0) printf("Статус:потверждение выключено\n");
	    else printf("Статус:потверждение включено\n");
		}
	     }	  
	     
	         }else if(strcasecmp(argv[1], "setupspeed") == 0){ // -----------установка скорости
      int speed;
   
             if(argc > 2) {
	      speed= atoi(argv[2]); // задать скорость
	
	    cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 111, speed, 0, buffer, sizeof(buffer), 5000);
	
	  printf("Вы установили скорость %s\n",speeddata[speed-1]);
	 
	     } else { // показать скорость
	       
	       cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 111, 255, 0, buffer, sizeof(buffer), 5000);
	        if (buffer[0]==-1) printf("Установлена скорость по умолчанию: 1MBit/s\n");
	        else printf("Установлена скорость: %s,%d\n",speeddata[buffer[0]-1],buffer[0]);
		printf("Установка скорости:\n1-2MBit/s\n2-1MBit/s\n3-250kBit/s\n");
	     }
	     
	        }else if(strcasecmp(argv[1], "setupcount") == 0){ // -----------установка количества канала
      int ch;
      
             if(argc > 2) {
	       ch= atoi(argv[2]); // задать количество клиентов
	
	    cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 112, ch, 0, buffer, sizeof(buffer), 5000);
	    if (buffer[0]==50) printf("Ошибка установки количества клиентов.\n");
	    else printf("Вы установлили клиентов %d\n",buffer[0]);
	    
	     } else { // показать количество клиентов
	       
	       cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 112, 255, 0, buffer, sizeof(buffer), 5000);
	        if (buffer[0]==-1) printf("Количество клиентов по умолчанию: 1\n");
	        else printf("Количество клиентов: %d\n",buffer[0]);
	     }
	     
      }else if(strcasecmp(argv[1], "readreg") == 0){  
      int reg= atoi(argv[2]);
      cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 115, reg, 0, buffer, sizeof(buffer), 5000);
	printf("Ответ: %d %08X\n\n",buffer[0],BIN(buffer[0]));
	
      }else if(strcasecmp(argv[1], "writereg") == 0){  
      int reg= atoi(argv[2]);
      int regdata= atoi(argv[3]);
      cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 116, reg+regdata*256, 0, buffer, sizeof(buffer), 5000);
  
#if ENABLE_TEST
    }else if(strcasecmp(argv[1], "test") == 0){
        int i;
        srandomdev();
        for(i = 0; i < 50000; i++){
            int value = random() & 0xffff, index = random() & 0xffff;
            int rxValue, rxIndex;
            if((i+1) % 100 == 0){
                fprintf(stderr, "\r%05d", i+1);
                fflush(stderr);
            }
            cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, CUSTOM_RQ_ECHO, value, index, buffer, sizeof(buffer), 5000);
            if(cnt < 0){
                fprintf(stderr, "\nUSB error in iteration %d: %s\n", i, usb_strerror());
                break;
            }else if(cnt != 4){
                fprintf(stderr, "\nerror in iteration %d: %d bytes received instead of 4\n", i, cnt);
                break;
            }
            rxValue = ((int)buffer[0] & 0xff) | (((int)buffer[1] & 0xff) << 8);
            rxIndex = ((int)buffer[2] & 0xff) | (((int)buffer[3] & 0xff) << 8);
            if(rxValue != value || rxIndex != index){
                fprintf(stderr, "\ndata error in iteration %d:\n", i);
                fprintf(stderr, "rxValue = 0x%04x value = 0x%04x\n", rxValue, value);
                fprintf(stderr, "rxIndex = 0x%04x index = 0x%04x\n", rxIndex, index);
            }
        }
        fprintf(stderr, "\nTest completed.\n");
#endif /* ENABLE_TEST */
    }else{
        usage(argv[0]);
        exit(1);
    }
    usb_close(handle);
    return 0;
}

