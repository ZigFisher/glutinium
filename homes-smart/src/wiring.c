#include "wiring.h"
#include <string.h>



void
send_usb(uint8_t cmd) 
{
usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, cmd, 0, 0, buffer, sizeof(buffer), 5000);

}



void
wiring_init() 
{
send_usb(41);
sleep(1);
}


uint8_t wr_spi (uint8_t data){
  
usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 2, data, 0, buffer, sizeof(buffer), 5000);
 
  return buffer[0];
}

void r_spi_array (){ // прием массива из 32 байт
  
send_usb(51);

}

void w_spi_array (uint8_t data1,uint8_t data2,uint8_t data3,uint8_t data4){ // отправка массива из 4 байт
  
usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 52, data1+data2*256, data3+data4*256, buffer, sizeof(buffer), 5000);

}



void w_spi_one (uint8_t data){ // функция отправки одного байта + SS !
  
usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 3, data, 0, buffer, sizeof(buffer), 5000);

}

uint8_t wr_spi_one (uint8_t data){ // функция отправки/приема одного байта + SS !
  
usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 4, data, 0, buffer, sizeof(buffer), 5000);
 
  return buffer[0];
}




uint8_t
wiring_write_then_read(uint8_t* out, uint16_t out_len, 
	               uint8_t* in, uint16_t in_len)
{
int i;

send_usb(21);

  
if (NULL != out) for (i=0;i<out_len;i++) // отправить

#if 1
  if (i+4<= out_len) { // отправка 4 байта за раз
//printf ("data %d\n",i);
  w_spi_array(out[i],out[i+1],out[i+2],out[i+3]);
  i+=3;
  }
else 
#endif
wr_spi(out[i]);

if (NULL != in) for (i=0;i<in_len;i++)
#if 1
 if (i+32<= in_len) { // прием 32 байта за раз
   
  r_spi_array();
  
  memcpy(&in[i],buffer, 32);
  i+=31;
  }
  
 else
#endif
 in[i]=wr_spi(0); // прочитать


send_usb(20);


return in_len+out_len;
}






void
wiring_set_gpio_value(uint8_t state)
{
send_usb(10+state);
}

void
wiring_destroy()
{
send_usb(40);
}
