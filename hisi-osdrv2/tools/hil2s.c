#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <sys/mman.h>

extern int errno;

#include "memmap.h"
#include "hi.h"
#include "strfunc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#define OPEN_FILE "/dev/mem"
#define CACHE_STAT_SIG SIGUSR1

#define PAGE_SIZE_MASK 0xfffff000

#define TIMER_INTERVAL 1
#define CACHE_ADDR 0x20700000
#define CACHE_MAP_LENGTH 0x1000

#define L2_INT_COUNTER2 0x0608
#define L2_INT_COUNTER3 0x060C
#define L2_INT_COUNTER4 0x0610
#define L2_INT_COUNTER5 0x0614
#define L2_INT_COUNTER6 0x0618
#define L2_INT_COUNTER7 0x061C

#define L2_EXT_COUNTER0 0x0700
#define L2_EXT_COUNTER1 0x0704
#define L2_EXT_COUNTER2 0x0708

#define reg_read(addr) (*(volatile unsigned int *)(addr))
#define reg_write(addr, val) (*(volatile unsigned int *)(addr) = (val))

static unsigned int *cache_addr;
static int fd;
static struct sigaction sa;
static timer_t tt;
static unsigned int timer_interval = TIMER_INTERVAL;
static unsigned int reg_value;

#define IO_ADDRESS(addr) ((unsigned char *)(cache_addr) + addr)
#define UNSIGNED_INT_MAX (~0x0)
#define CACHE_RESET_TMP(data) (UNSIGNED_INT_MAX - data)

static void reset_l1_reg()
{
	reg_write(IO_ADDRESS(L2_EXT_COUNTER0), UNSIGNED_INT_MAX);	
	reg_write(IO_ADDRESS(L2_EXT_COUNTER1), UNSIGNED_INT_MAX);	
	reg_write(IO_ADDRESS(L2_EXT_COUNTER2), UNSIGNED_INT_MAX);	
}

static void reset_l2_reg()
{
	reg_write(IO_ADDRESS(L2_INT_COUNTER2), UNSIGNED_INT_MAX);	
	reg_write(IO_ADDRESS(L2_INT_COUNTER3), UNSIGNED_INT_MAX);	
	reg_write(IO_ADDRESS(L2_INT_COUNTER4), UNSIGNED_INT_MAX);	
	reg_write(IO_ADDRESS(L2_INT_COUNTER5), UNSIGNED_INT_MAX);	
	reg_write(IO_ADDRESS(L2_INT_COUNTER6), UNSIGNED_INT_MAX);	
	reg_write(IO_ADDRESS(L2_INT_COUNTER7), UNSIGNED_INT_MAX);	
}

static void cache_statistic(int n)
{
	unsigned int request = 0;
	unsigned int total = 0;

	float data_read = 0.0f;
	float data_write = 0.0f;
	float instruct_read = 0.0f;
	float l1_i_read = 0.0f;
	float l1_data = 0.0f;

	request = reg_read(IO_ADDRESS(L2_INT_COUNTER2));
	total = reg_read(IO_ADDRESS(L2_INT_COUNTER3));
	data_read = ((float)CACHE_RESET_TMP(request) / (float)CACHE_RESET_TMP(total)) * 100;
	
	request = reg_read(IO_ADDRESS(L2_INT_COUNTER4));
	total = reg_read(IO_ADDRESS(L2_INT_COUNTER5));
	data_write = ((float)CACHE_RESET_TMP(request) / (float)CACHE_RESET_TMP(total)) * 100;
	
	request = reg_read(IO_ADDRESS(L2_INT_COUNTER6));
	total = reg_read(IO_ADDRESS(L2_INT_COUNTER7));
	instruct_read = ((float)CACHE_RESET_TMP(request) / (float)CACHE_RESET_TMP(total)) * 100;

	request = reg_read(IO_ADDRESS(L2_EXT_COUNTER1));
	total = reg_read(IO_ADDRESS(L2_EXT_COUNTER2));
	l1_data = (1.0f - (float)CACHE_RESET_TMP(request) / (float)CACHE_RESET_TMP(total)) * 100;
	printf("%0.2f%% %8.2f%% %8.2f%% %8.2f%%\n", data_read, data_write, instruct_read, l1_data);
	reset_l1_reg();
	reset_l2_reg();
	return;	
}

static void cache_ctrl_c(int n)
{
	timer_delete(tt);	
	munmap(cache_addr, CACHE_MAP_LENGTH);
	close(fd);
	_exit(0);
}


//int main(int argc, char *argv[])
HI_RET hil2s(int argc , char* argv[])
{
	unsigned int phy_addr_in_page;
	struct sigevent sev;
	struct itimerspec tspec;
	struct sigevent timer_event;
	unsigned int second = 0;

	if (argc == 2) {
		second = atoi(argv[1]);
	} else if (argc == 1) {
		second = TIMER_INTERVAL;
	} else {
ERR:
		printf("L2 cache statistic usage like this:\n");
		printf("\thil2s [second=1,2,3...]\n");
		printf("\teg. hil2s 2\n");
		return -1;
	}

	timer_interval = second;

	fd = open(OPEN_FILE, O_RDWR | O_SYNC);
	if (-1 == fd) {
		perror("cache statistic open failed:");
		goto ERR0;
	}

	phy_addr_in_page = CACHE_ADDR & PAGE_SIZE_MASK;
	cache_addr = (unsigned int *)mmap(NULL, CACHE_MAP_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, phy_addr_in_page);

	if (cache_addr == MAP_FAILED) {
		printf("cache statistic mmap failed.\n");
		goto ERR1;
	}

	sa.sa_flags = SA_RESTART;
	sa.sa_handler = cache_statistic;

	sigemptyset(&sa.sa_mask);

	if (sigaction(CACHE_STAT_SIG, &sa, NULL)) {
		printf("cache statistic install signal failed.\n");
		goto ERR1;
	}

	timer_event.sigev_notify = SIGEV_SIGNAL;
	timer_event.sigev_signo = CACHE_STAT_SIG;
	timer_event.sigev_value.sival_ptr = (void *)&tt;

	if (timer_create(CLOCK_REALTIME, &timer_event, &tt) < 0) {
		printf("cache statistic timer create failed.\n");
		goto ERR2;
	}

	tspec.it_value.tv_sec = timer_interval;
	tspec.it_value.tv_nsec = 0;
	tspec.it_interval.tv_sec = timer_interval;
	tspec.it_interval.tv_nsec = 0;

	if (timer_settime(tt, TIMER_ABSTIME, &tspec, NULL) < 0) {
		printf("cache statistic set timer failed.\n");
		goto ERR3;
	}
	
	reset_l1_reg();
	reset_l2_reg();

	printf("===== L2 cache statistic =====\n");
	printf("%s %10s %8s %8s\n", "L2 dr", "L2 dw", "L1 i", "L1 d");

	struct sigaction sa_ctrl_c;
	sa_ctrl_c.sa_flags = SA_RESTART;
	sa_ctrl_c.sa_handler = cache_ctrl_c;
	sigemptyset(&sa_ctrl_c.sa_mask);

	if (sigaction(SIGINT, &sa_ctrl_c, NULL)) {
		printf("cache statistic catch SIGINT signal failed.\n");
		goto ERR3;
	}

	while (1)
		sleep(1);
	return 0;

ERR3:
	timer_delete(tt);
ERR2:
	munmap(cache_addr, CACHE_MAP_LENGTH);
ERR1:
	close(fd);
ERR0:
	return -1;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
