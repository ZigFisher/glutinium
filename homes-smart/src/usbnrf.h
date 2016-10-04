/* Настройка массивов принятых данных
 * Страница проекта http://homes-smart.ru/index.php/oborudovanie/bez-provodov-2-4-ggts/50-nrf24l01-usb
byted -однобайтовое число
intd - 2 байтовое число
uintd - 2 байтовое ПОЛОЖИТЕЛЬНОЕ(безнаковое) число
longd - 4 байтовое число
ulongd - 4 байтовое ПОЛОЖИТЕЛЬНОЕ(безнаковое) число
floatd - тип float
end -обязательная метка окончания массива
Первый байт массива - это всегда номер клиента.
 */

int dtstr[][20]={
  {byted,intd,byted,intd,longd,floatd,floatd,end}, // клиент 1
  {byted,intd,byted,intd,longd,floatd,floatd,end}, // клиент 2
  {byted,intd,byted,intd,longd,floatd,floatd,end}, // клиент 3 
  };

