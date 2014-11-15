/*
 * flyavr.c
 *
 * Device driver for FLYAVR
 *
 * Copyright (C) 2008 Flyrouter Team, Vladislav Moskovets
 *
 */

#include <linux/config.h>
#include <linux/module.h>
#include <linux/version.h>

#include <linux/kernel.h>
#include <linux/poll.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/miscdevice.h>
#include <linux/proc_fs.h>
#include <linux/timer.h>
#include <linux/random.h>


#define DEBUG 5
#define FLYAVR_I2C_SLAVE_ADDR 0x10
#define CRC_TRY_COUNT 5

#define CMD_GET_EEPROM		(0x01)
#define CMD_SET_EEPROM		(0x02)
#define CMD_GET_VERSION		(0x10)
#define CMD_GET_HW			(0x11)
#define CMD_GET_COUNTER		(0x1C)
#define CMD_GET_REBOOTS		(0x1D)
#define CMD_GET_CRC1		(0x20)
#define CMD_PIN1_ON			(0x30)
#define CMD_PIN1_OFF		(0x31)
#define CMD_PIN2_ON			(0x32)
#define CMD_PIN2_OFF		(0x33)
#define CMD_PIN3_ON			(0x34)
#define CMD_PIN3_OFF		(0x35)


#define PROC_FLYAVR_NAME	"driver/flyavr"


#define dbg  if(DEBUG >= 4)printk
#define info if(DEBUG >= 3)printk
#define warn if(DEBUG >= 2)printk

static struct proc_dir_entry *flyavr_proc_entry;

struct timer_list flyavr_timer;
static __u8 recv_buf[16];
static __u8 send_buf[16];

/* crc function definitions */
__u16 crc_ccitt_update (__u16 crc, __u8 data);
__u16 crc_xmodem_update (__u16 crc, __u8 data);
void crc1(__u8* buf);

#if DEBUG
static unsigned int debug = DEBUG;
#else
#define debug 0	/* gcc will remove all the debug code for us */
#endif

static unsigned short slave_address = FLYAVR_I2C_SLAVE_ADDR;

struct i2c_driver flyavr_driver;
struct i2c_client *flyavr_i2c_client = 0;

static unsigned short ignore[] = { I2C_CLIENT_END };
static unsigned short normal_addr[] = { FLYAVR_I2C_SLAVE_ADDR, I2C_CLIENT_END };

static struct i2c_client_address_data addr_data = {
	normal_i2c:		normal_addr,
	normal_i2c_range:	ignore,
	probe:			ignore,
	probe_range:		ignore,
	ignore: 		ignore,
	ignore_range:		ignore,
	force:			ignore,
};

static int flyavr_ioctl( struct inode *, struct file *, unsigned int, unsigned long);
static int flyavr_open(struct inode *inode, struct file *file);
static int flyavr_release(struct inode *inode, struct file *file);

static struct file_operations fops = {
	owner:		THIS_MODULE,
	ioctl:		flyavr_ioctl,
	open:		flyavr_open,
	release:	flyavr_release,
};

static struct miscdevice flyavr_miscdev = {
	MISC_DYNAMIC_MINOR,
	"flyavr",
	&fops
};

static int flyavr_probe(struct i2c_adapter *adap);
static int flyavr_detach(struct i2c_client *client);
static int flyavr_command(struct i2c_client *client, unsigned int cmd, void *arg);

struct i2c_driver flyavr_driver = {
	name:		"FLYAVR",
	//id:			I2C_DRIVERID_FLYAVR,
	flags:		I2C_DF_NOTIFY,
	attach_adapter: flyavr_probe,
	detach_client:	flyavr_detach,
	command:	flyavr_command
};

static spinlock_t flyavr_lock = SPIN_LOCK_UNLOCKED;

static int
flyavr_attach(struct i2c_adapter *adap, int addr, unsigned short flags,int kind)
{
	struct i2c_client *c;
	unsigned char buf[1], ad[1] = { 7 };
	struct i2c_msg msgs[2] = {
		{ addr	, 0,	    1, ad  },
		{ addr	, I2C_M_RD, 1, buf }
	};
	int ret;

	c = (struct i2c_client *)kmalloc(sizeof(*c), GFP_KERNEL);
	if (!c)
		return -ENOMEM;

	strcpy(c->name, "FLYAVR");
	c->id		= flyavr_driver.id;
	c->flags	= 0;
	c->addr 	= addr;
	c->adapter	= adap;
	c->driver	= &flyavr_driver;
	c->data 	= NULL;

	ret = i2c_transfer(c->adapter, msgs, 2);

	if ( 2 != ret )
		printk ("flyavr_attach(): i2c_transfer() returned %d.\n",ret);

	flyavr_i2c_client = c;
	return i2c_attach_client(c);
}

static int
flyavr_probe(struct i2c_adapter *adap)
{
	return i2c_probe(adap, &addr_data, flyavr_attach);
}

static int
flyavr_detach(struct i2c_client *client)
{
	i2c_detach_client(client);
	return 0;
}

static int
flyavr_command(struct i2c_client *client, unsigned int cmd, void *arg)
{
	switch (cmd) {
#if 0
	case FLYAVR_GETDATETIME:
		return flyavr_get_datetime(client, arg);

	case FLYAVR_SETTIME:
		return flyavr_set_datetime(client, arg, 0);

	case FLYAVR_SETDATETIME:
		return flyavr_set_datetime(client, arg, 1);

	case FLYAVR_GETCTRL:
		return flyavr_get_ctrl(client, arg);

	case FLYAVR_SETCTRL:
		return flyavr_set_ctrl(client, arg);

	case FLYAVR_MEM_READ:
		return flyavr_read_mem(client, arg);

	case FLYAVR_MEM_WRITE:
		return flyavr_write_mem(client, arg);
#endif
	default:
		return -EINVAL;
	}
}

static int
flyavr_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int
flyavr_release(struct inode *inode, struct file *file)
{
	return 0;
}

static int
flyavr_ioctl( struct inode *inode, struct file *file,
		unsigned int cmd, unsigned long arg)
{
	//unsigned long	flags;
	int status = 0;

#if 0
	switch (cmd) {
		default:
	}
#endif

	return status;
}



/* ##########################################################################
 *
 *
 *      I2C messaging
 *
 *
 * ########################################################################## */

int SendReceiveBlock(__u8 *send_buf, size_t send_buf_size, __u8 *recv_buf, size_t recv_buf_size)
{
	int ret;
	struct i2c_msg msgs[2] = {
		{ FLYAVR_I2C_SLAVE_ADDR, 0,			send_buf_size, send_buf },
		{ FLYAVR_I2C_SLAVE_ADDR, I2C_M_RD,	recv_buf_size, recv_buf }
	};

	//memset(recv_buf, 0, sizeof(recv_buf));
	//spin_lock_irqsave(&flyavr_lock, flags);
	ret = i2c_transfer(flyavr_i2c_client->adapter, msgs, 2);
	//spin_unlock_irqrestore(&flyavr_lock,flags);

	//dbg("Send:");
	//for(i=0; i<send_buf_size; i++)
	//	dbg(" %2x", (__u8)send_buf[i]);
	//dbg("   Read:");
	//for(i=0; i<recv_buf_size; i++)
	//	dbg(" %2x", (__u8)recv_buf[i]);
	//dbg("\n");

#if 0
	if (ret < 0) {
		warn("Error: Sending/Reading error:%d\n", ret);
		return 0;
	}
#endif
	return ret;
}


int flyavr_send_byte(unsigned char cmd)
{
	send_buf[0] = cmd;
	return i2c_master_send(flyavr_i2c_client, send_buf, 1);
}

unsigned int u16from_buf(unsigned char *buf)
{
	return ( buf[0] |  (buf[1] << 8) );
}
unsigned int u32from_buf(unsigned char *buf)
{
	return ( buf[0] |  (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24) );
}

void u32to_buf(unsigned int num, unsigned char* buf)
{
	buf[0] = (num & 0xFF);
	buf[1] = ((num >> 8) & 0xFF);
	buf[2] = ((num >> 16) & 0xFF);
	buf[3] = ((num >> 24) & 0xFF);
}

int get_ver()
{
	send_buf[0] = CMD_GET_VERSION;
	SendReceiveBlock(send_buf, 1, recv_buf, 1);
	dbg("Version: 0x%x\n", recv_buf[0]);
	return recv_buf[0];
}

int get_hw()
{
	send_buf[0] = CMD_GET_HW;
	SendReceiveBlock(send_buf, 1, recv_buf, 1);
	dbg("Hardware: %d\n", recv_buf[0]);
	return recv_buf[0];
}

int get_counter()
{
	__u32 counter = 0;
	send_buf[0] = CMD_GET_COUNTER;
	SendReceiveBlock(send_buf, 1, recv_buf, 4);
	counter = u32from_buf(recv_buf);

	dbg("Counter: %u\n", counter);
	return counter;
}

int get_eeprom(__u8 offset)
{
	send_buf[0] = CMD_GET_EEPROM;
	send_buf[1] = offset;
	SendReceiveBlock(send_buf, 2, recv_buf, 1);
	dbg("EEPROM[%d]: %d\n", offset, recv_buf[0]);
	return recv_buf[0];
}

int set_eeprom(__u8 offset, __u8 value)
{
	send_buf[0] = CMD_SET_EEPROM;
	send_buf[1] = offset;
	send_buf[2] = value;
	return i2c_master_send(flyavr_i2c_client, send_buf, 3);
}

int get_reboots(unsigned char wdt)
{
	send_buf[0] = CMD_GET_REBOOTS;
	SendReceiveBlock(send_buf, 1, recv_buf, 8);
	dbg("Reboots: %d\n", u32from_buf(recv_buf));
	dbg("WDT Reboots: %d\n", u32from_buf(recv_buf+2));
	return wdt?u16from_buf(recv_buf):u16from_buf(recv_buf+2);
}

int get_crc1(unsigned int num)
{
	__u8 crc_buf[4];
	int flyavr_crc = 0, calc_crc = 0;
	send_buf[0] = CMD_GET_CRC1;
	u32to_buf(num, &send_buf[1]);
	SendReceiveBlock(send_buf, 5, recv_buf, 4);
	flyavr_crc = u32from_buf(recv_buf);
	//printf("CRC1 FROM FLYAVR: 0x%X\n", flyavr_crc);

	u32to_buf(num, crc_buf);
	//for (i = 0; i < 4; i++) info("%2X ", crc_buf[i]); info("\n");
	crc1(crc_buf);

	//for (i = 0; i < 4; i++) info("%2X ", crc_buf[i]); info("\n");
	calc_crc = u32from_buf(crc_buf);
	//printf("CRC1  CALCULATED: 0x%X\n", calc_crc);
	if ( calc_crc == flyavr_crc ) {
		dbg("ok ");
		return 1;
	} else {
		info("fail %X   %X\n", flyavr_crc, calc_crc);
	}
	return 0;
}

inline int crc_check()
{
	unsigned int num;
	get_random_bytes(&num, sizeof(num));
	return get_crc1(num);
}

static int fail_counter=0;
static int flyavr_proc_output( char *buf)
{
	//int ret=0;
	char *p = buf;
	p += sprintf(p, "FLYAVR\n");

	p += sprintf(p, "I2C Counter : %d\n", get_counter());
	p += sprintf(p, "HW	 : %03d\n", get_hw());
	p += sprintf(p, "Version	 : 0x%02x\n", get_ver());
	p += sprintf(p, "CRC	 : %s\n", crc_check()?"ok":"fail");
	p += sprintf(p, "Fails	 : %d\n", fail_counter);
#if 0
	p += sprintf(p, "Reboots	 : %d\n", get_reboots(0));
	p += sprintf(p, "WDT Reboots : %d\n", get_reboots(1));
#endif

#if 0
	p += sprintf(p, "RAM dump:\n");
	text[8]='\0';
	for( i=0; i<FLYAVR_RAM_SIZE; i++)
	{
		p += sprintf(p, "%02X ", ram[i]);

		if( (ram[i] < 32) || (ram[i]>126)) ram[i]='.';
		text[i%8] = ram[i];
		if( (i%8) == 7) p += sprintf(p, "%s\n",text);
	}
#endif
	p += sprintf(p, "\n");
	return	p - buf;
}

static int flyavr_read_proc(char *page, char **start, off_t off,
		int count, int *eof, void *data)
{
	int len = flyavr_proc_output (page);
	if (len <= off+count) *eof = 1;
	*start = page + off;
	len -= off;
	if (len>count) len = count;
	if (len<0) len = 0;
	return len;
}

static int flyavr_write_proc(struct file *file, const char *buffer, unsigned long count,
				   void *data)
{
	unsigned char pin;
	char state=1;
	if ( count >= 7 && buffer[0] == 'p' && buffer[1] == 'i' && buffer[2] == 'n' && buffer[3] >= '1' && buffer[4] <= '3' && buffer[5] == 'o' ) {
		pin = buffer[3] - '0';	

		if ( buffer[6] == 'f' ) 
			state = 0;
		dbg("pin=%d, state=%d\n", pin, state);
		switch (pin) {
			case 1: flyavr_send_byte(state ? CMD_PIN1_ON : CMD_PIN1_OFF);
				break;
			case 2: flyavr_send_byte(state ? CMD_PIN2_ON : CMD_PIN2_OFF);
				break;
			case 3: flyavr_send_byte(state ? CMD_PIN3_ON : CMD_PIN3_OFF);
				break;
			default:
					warn("flyavr: bad pin number");
		}
	} else {
		printk ("flyavr: bad syntax\n");
		dbg("0=%c 1=%c 2=%c 3=%c 4=%c 5=%c 6=%c 7=%c\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7]);
	}
	
	return (count);
	return count;
}


static void flyavr_timer_func(unsigned long data)
{
	static unsigned int timer_counter=0;

	//dbg(KERN_DEBUG "flyavr timer fire\n");
	if ( !crc_check() ) {
		info("\nFLYAVR CRC CHECK FAIL\n");
		// some code for diff
		fail_counter++;
	} else {
		// some code for diff
		fail_counter = 0;
	}
		
	if ( fail_counter >= CRC_TRY_COUNT ) {
		// some code for diff
		// some code for diff
		// some code for diff
		// some code for diff
		info("\nREBOOTING\n");
		panic("flyavr: too many errors\n");
	}
	flyavr_timer.expires = jiffies + ((timer_counter < CRC_TRY_COUNT)? (5 * HZ) : (30 * HZ) );
	add_timer(&flyavr_timer);
}

static __init int flyavr_init(void)
{
	int retval=0;

    dbg("FLYAVR: init...\n");
	init_timer(&flyavr_timer);
	flyavr_timer.function = flyavr_timer_func;
	flyavr_timer.data = 1;
	flyavr_timer.expires = jiffies + (30 * HZ);
	add_timer(&flyavr_timer);
    dbg("FLYAVR: timer started...\n");

	if( slave_address != 0xffff)
	{
		normal_addr[0] = slave_address;
	}

	if( normal_addr[0] == 0xffff)
	{
		printk(KERN_ERR"I2C: Invalid slave address for FLYAVR (%#x)\n",
			normal_addr[0]);
		return -EINVAL;
	}

    dbg("FLYAVR: add i2c driver... ");
	retval = i2c_add_driver(&flyavr_driver);
    dbg(" done\n");

	if (retval==0)
	{
		int i=CRC_TRY_COUNT;
		misc_register (&flyavr_miscdev);

#define PROC_FLYAVR_RW
#ifdef PROC_FLYAVR_RW
		flyavr_proc_entry = create_proc_entry(PROC_FLYAVR_NAME, 0644, NULL);
		if ( ! flyavr_proc_entry ) {
			remove_proc_entry(PROC_FLYAVR_NAME, &proc_root);
			printk(KERN_ALERT "Error: Could not initialize /proc/%s\n", PROC_FLYAVR_NAME);
			return -ENOMEM;
		}
		flyavr_proc_entry->read_proc = flyavr_read_proc;
		flyavr_proc_entry->write_proc = flyavr_write_proc;
		flyavr_proc_entry->owner = THIS_MODULE;
		flyavr_proc_entry->mode = S_IFREG | S_IRUGO;
		flyavr_proc_entry->uid = 0;
		flyavr_proc_entry->gid = 0;
		flyavr_proc_entry->size = 0;
#else
		create_proc_read_entry (PROC_FLYAVR_NAME, 0, 0, flyavr_read_proc, NULL);
#endif

		printk("FLYAVR driver successfully loaded\n");
		return 0;
	}
	return retval;
}

static __exit void flyavr_exit(void)
{
	remove_proc_entry (PROC_FLYAVR_NAME, NULL);
	misc_deregister(&flyavr_miscdev);
	i2c_del_driver(&flyavr_driver);
}


/* ##########################################################################
 *
 *
 *      CRC funcs
 *
 *
 * ########################################################################## */
inline __u8 lo8(__u16 n)
{
	return (n & 0xFF);
}

inline __u8 hi8(__u16 n)
{
	return (n >> 8);
}


__u16 crc_ccitt_update (__u16 crc, __u8 data)
{
	data ^= lo8 (crc);
	data ^= data << 4;

	crc =  ((((__u16)data << 8) | hi8 (crc)) ^ (__u8)(data >> 4) 
			^ ((__u16)data << 3));
	//dbg("CRC16_CCITT: %04x\n", crc);
	return crc;
}


__u16 crc_xmodem_update (__u16 crc, __u8 data)
{
	int i;

	crc = crc ^ ((__u16)data << 8);
	for (i=0; i<8; i++)
	{
		if (crc & 0x8000)
			crc = (crc << 1) ^ 0x1021;
		else
			crc <<= 1;
	}

	//dbg("CRC16_XMODEM: %04x\n", crc);
	return crc;
}

//
//   FLYAVR CODE
//
// crc16_xmodem = _crc_xmodem_update(0,             (  buf[0] ^ 0x55 ) ); // xor, 0x55 = 1010101b
// crc16_xmodem = _crc_xmodem_update(crc16_xmodem,  ( ~buf[1]        ) ); // not
// crc16_ccitt  = _crc_ccitt_update(crc16_xmodem,   ( ~buf[2]        ) ); // not
// crc16_ccitt  = _crc_ccitt_update(crc16_xmodem,   (  buf[3] ^ 0x66 ) ); // xor, 0x66 
// usiTwiTransmitByte( (crc16_xmodem >> 0) & 0xff );
// usiTwiTransmitByte( (crc16_ccitt >> 0) & 0xff );
// usiTwiTransmitByte( (crc16_xmodem >> 8) & 0xff );
// usiTwiTransmitByte( (crc16_ccitt >> 8) & 0xff );

void crc1(__u8* buf)
{
	__u16 crc16_xmodem;
	__u16 crc16_ccitt;

	crc16_xmodem = crc_xmodem_update(0,             (   buf[0] ^ 0x55 ) );
	crc16_xmodem = crc_xmodem_update(crc16_xmodem,  (  ~buf[1]        ) );
	crc16_ccitt  = crc_ccitt_update(crc16_xmodem,   (  ~buf[2]        ) );
	crc16_ccitt  = crc_ccitt_update(crc16_xmodem,   (   buf[3] ^ 0x66 ) );
	buf[0] = ( (crc16_xmodem >> 0) & 0xff );
	buf[1] = ( (crc16_ccitt >> 0)  & 0xff );
	buf[2] = ( (crc16_xmodem >> 8) & 0xff );
	buf[3] = ( (crc16_ccitt >> 8)  & 0xff );
}
module_init(flyavr_init);
module_exit(flyavr_exit);

MODULE_PARM (slave_address, "i");
MODULE_PARM_DESC (slave_address, "I2C slave address for FLYAVR.");

MODULE_AUTHOR ("FlyRouter Team 2009.");
MODULE_LICENSE("GPL");
