#ifndef __SENSOR_SPI_H__
#define	__SENSOR_SPI_H__
int ssp_write_alt(unsigned char devaddr, unsigned char addr, unsigned char data);
int ssp_read_alt(unsigned char devaddr, unsigned char addr, unsigned char* data);
#endif
