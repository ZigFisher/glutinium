#include <errno.h>
#include <string.h>

#include "wiring.h"

/* spi commands */
#define SPICMD_WREN      0x06
#define SPICMD_WRDIS     0x04
#define SPICMD_RDSR      0x05
#define SPICMD_WRSR      0x01
#define SPICMD_READ      0x03
#define SPICMD_PROGRAM   0x02
#define SPICMD_ERASEPAGE 0x52
#define SPICMD_ERASEALL  0x62
#define SPICMD_RDFPCR    0x89
#define SPICMD_RDISMB    0x85
#define SPICMD_ENDEBUG   0x86

#define FSR_RESERVED0 (1 << 0)
#define FSR_RESERVED1 (1 << 1)
#define FSR_RDISMB    (1 << 2)
#define FSR_INFEN     (1 << 3)
#define FSR_RDYN      (1 << 4)
#define FSR_WEN       (1 << 5)
#define FSR_STP       (1 << 6)
#define FSR_ENDEBUG   (1 << 7)

/* NVM Extended endurance data  pages: 32,33 */
#define NVM_NORMAL_PAGE0           34
#define NVM_NORMAL_PAGE0_INI_ADDR  0x4400
#define NVM_NORMAL_PAGE0_END_ADDR  0x45FF
#define NVM_NORMAL_PAGE1           35
#define NVM_NORMAL_PAGE1_INI_ADDR  0x4600
#define NVM_NORMAL_PAGE1_END_ADDR  0x47FF
#define NVM_NORMAL_NUMBER_OF_PAGES 2
#define NVM_NORMAL_MEM_SIZE        (NVM_NORMAL_NUMBER_OF_PAGES * NRF_PAGE_SIZE)

#define NRF_PAGE_SIZE     (512)
#define N_PAGES           (32)
#define MAX_FIRMWARE_SIZE (NRF_PAGE_SIZE * N_PAGES) /* 16Kb */
#define N_BYTES_FOR_WRITE (16)
#define N_BYTES_FOR_READ  (16)
#define NRF_SPI_SPEED_HZ  (4500 * 1000) /* 4.5Mhz */

#define GPIO_PROG		WIRING_NRF_PROG_PIN // wiringPi or GPIO 24 in BCM
#define GPIO_RESET		WIRING_NRF_RESET_PIN // wiringPi or GPIO 25 in BCM 

#define debug(fmt, args...) \
{ \
	printf("[" NAME "] %s: " fmt "\n", __func__, ##args); \
}

/* Public Interfaces */

void nrf24le1_init();

void enable_program(uint8_t);

ssize_t uhet_read(char* buf, size_t count, unsigned long *off);
ssize_t uhet_write(char* buf, size_t count, unsigned long *off);

ssize_t da_test_show(void);
ssize_t da_infopage_show(char *buf);
ssize_t da_nvm_normal_show(char* buf);
ssize_t da_enable_program_show(void);

ssize_t da_infopage_store(const char *, size_t);
ssize_t da_nvm_normal_store(const char *, size_t);
ssize_t da_enable_program_store(uint8_t state);
ssize_t da_erase_all_store();
