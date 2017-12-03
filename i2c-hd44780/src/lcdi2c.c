#include "lcdi2c.h"

#define CRIT_BEG(d, error) if(down_interruptible(&d->sem)) return -error
#define CRIT_END(d) up(&d->sem)

static uint busno = 0;      //I2C Bus number
static uint address = DEFAULT_CHIP_ADDRESS; //Device address
static uint topo = LCD_DEFAULT_ORGANIZATION;
static uint cursor = 1;
static uint blink = 1;
static IOCTLDescription_t ioctls[] = {
  { .ioctlcode = LCD_IOCTL_GETCHAR, .name = "GETCHAR", },
  { .ioctlcode = LCD_IOCTL_SETCHAR, .name = "SETCHAR", },
  { .ioctlcode = LCD_IOCTL_GETPOSITION, .name = "GETPOSITION" },
  { .ioctlcode = LCD_IOCTL_SETPOSITION, .name = "SETPOSITION" },
  { .ioctlcode = LCD_IOCTL_RESET, .name = "RESET" },
  { .ioctlcode = LCD_IOCTL_HOME, .name = "HOME" },
  { .ioctlcode = LCD_IOCTL_GETBACKLIGHT, .name = "GETBACKLIGHT" },
  { .ioctlcode = LCD_IOCTL_SETBACKLIGHT, .name = "SETBACKLIGHT" },
  { .ioctlcode = LCD_IOCTL_GETCURSOR, .name = "GETCURSOR" },
  { .ioctlcode = LCD_IOCTL_SETCURSOR, .name = "SETCURSOR" },
  { .ioctlcode = LCD_IOCTL_GETBLINK,  .name = "GETBLINK" },
  { .ioctlcode = LCD_IOCTL_SETBLINK,  .name = "SETBLINK" },
  { .ioctlcode = LCD_IOCTL_SCROLLHZ,  .name = "SCROLLHZ" },
  { .ioctlcode = LCD_IOCTL_GETCUSTOMCHAR, .name = "GETCUSTOMCHAR" },
  { .ioctlcode = LCD_IOCTL_SETCUSTOMCHAR, .name = "SETCUSTOMCHAR" },
  { .ioctlcode = LCD_IOCTL_CLEAR, .name = "CLEAR" },
};


static struct i2c_client *client;
static struct i2c_adapter *adapter;
static LcdData_t *data;
static int major = DEVICE_MAJOR;
static int minor = 0;
static struct class *lcdi2c_class = NULL;
static struct device *lcdi2c_device = NULL;

module_param(busno, uint, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
module_param(address, uint, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
module_param_array(pinout, uint, NULL, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
module_param(cursor, uint, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
module_param(blink, uint, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
module_param(topo, uint, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
module_param(major, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

MODULE_PARM_DESC(busno, " I2C Bus number, default 0");
MODULE_PARM_DESC(address, " LCD I2C Address, default 0x27");
MODULE_PARM_DESC(pinout, " I2C module pinout configuration, eight "
                         "numbers\n\t\trepresenting following LCD module"
                         "pins in order: RS,RW,E,D4,D5,D6,D7,\n"
			 "\t\tdefault 0,1,2,3,4,5,6,7");
MODULE_PARM_DESC(cursor, " Show cursor at start 1 - Yes, 0 - No, default 1");
MODULE_PARM_DESC(blink, " Blink cursor 1 - Yes, 0 - No, defualt 1");
MODULE_PARM_DESC(major, " Device major number, default 0");
MODULE_PARM_DESC(topo, " Display organization, following values are currently supported:\n"
                        "\t\t0 - 40x2\n"
                        "\t\t1 - 20x4\n"
                        "\t\t2 - 20x2\n"
                        "\t\t3 - 16x4\n"
                        "\t\t4 - 16x2\n"
                        "\t\t5 - 16x1 Type 1\n"
                        "\t\t6 - 16x1 Type 2\n"
			"\t\t7 - 8x2\n"
                        "\t\tDefault set to 16x2");

static int lcdi2c_open(struct inode *inode, struct file *file)
{
    CRIT_BEG(data, EBUSY);

    data->deviceopencnt++;
    data->devicefileptr = 0;
    try_module_get(THIS_MODULE);
    CRIT_END(data);

    return SUCCESS;
}

static int lcdi2c_release(struct inode *inode, struct file *file)
{
    CRIT_BEG(data, EBUSY);

    data->deviceopencnt--;

    module_put(THIS_MODULE);
    CRIT_END(data);
    return SUCCESS;
}

static ssize_t lcdi2c_fopread(struct file *file, char __user *buffer,
			   size_t length, loff_t *offset)
{
    u8 i = 0;

    CRIT_BEG(data, EBUSY);
    if (data->devicefileptr == (data->organization.columns * data->organization.rows))
      return 0;

    while(i < length && i < (data->organization.columns * data->organization.rows))
    {
      put_user(data->buffer[i], buffer++);
      data->devicefileptr++;
      i++;
    }

    i %= (data->organization.columns * data->organization.rows);
    ITOP(data, i, data->column, data->row);
    lcdsetcursor(data, data->column, data->row);
    (*offset) = i;
    CRIT_END(data);

    return i;
}

static ssize_t lcdi2c_fopwrite(struct file *file, const char __user *buffer,
			    size_t length, loff_t *offset)
{
    u8 i, str[81];

    CRIT_BEG(data, EBUSY);

    for(i = 0; i < length && i < (data->organization.columns * data->organization.rows); i++)
      get_user(str[i], buffer + i);
    str[i] = 0;
    (*offset) = lcdprint(data, str);

    CRIT_END(data);
    return i;
}

loff_t lcdi2c_lseek(struct file *file, loff_t offset, int orig)
{
  u8 memaddr, oldoffset;

  CRIT_BEG(data, EBUSY);
  memaddr = data->column + (data->row * data->organization.columns);
  oldoffset = memaddr;
  memaddr = (memaddr + (u8)offset) % (data->organization.rows * data->organization.columns);
  data->column =  (memaddr % data->organization.columns);
  data->row = (memaddr / data->organization.columns);
  lcdsetcursor(data, data->column, data->row);
  CRIT_END(data);

  return oldoffset;
}

static long lcdi2c_ioctl(struct file *file,
			unsigned int ioctl_num,
			unsigned long arg)
{

  char *buffer = (char*)arg, ccb[10];
  u8 memaddr, i, ch;
  long status = SUCCESS;

  CRIT_BEG(data, EAGAIN);

  switch (ioctl_num)
  {
    case LCD_IOCTL_SETCHAR:
      get_user(ch, buffer);
      memaddr = (1 + data->column + (data->row * data->organization.columns)) % LCD_BUFFER_SIZE;
      lcdwrite(data, ch);
      data->column = (memaddr % data->organization.columns);
      data->row = (memaddr / data->organization.columns);
      lcdsetcursor(data, data->column, data->row);
      break;
    case LCD_IOCTL_GETCHAR:
      memaddr = (data->column + (data->row * data->organization.columns)) % LCD_BUFFER_SIZE;
      ch = data->buffer[memaddr];
      put_user(ch, buffer);
      break;
    case LCD_IOCTL_GETPOSITION:
      printk(KERN_INFO "GETPOSITION called\n");
      put_user(data->column, buffer);
      put_user(data->row, buffer+1);
      break;
    case LCD_IOCTL_SETPOSITION:
      get_user(data->column, buffer);
      get_user(data->row, buffer+1);
      lcdsetcursor(data, data->column, data->row);
      break;
    case LCD_IOCTL_RESET:
      get_user(ch, buffer);
      if (ch == '1')
	lcdinit(data, data->organization.topology);
      break;
    case LCD_IOCTL_HOME:
      printk(KERN_INFO "HOME called\n");
      get_user(ch, buffer);
      if (ch == '1')
	lcdhome(data);
      break;
    case LCD_IOCTL_GETCURSOR:
      put_user(data->cursor ? '1' : '0', buffer);
      break;
    case LCD_IOCTL_SETCURSOR:
      get_user(ch, buffer);
      lcdcursor(data, (ch == '1'));
      break;
    case LCD_IOCTL_GETBLINK:
      put_user(data->blink ? '1' : '0', buffer);
      break;
    case LCD_IOCTL_SETBLINK:
      get_user(ch, buffer);
      lcdblink(data, (ch == '1'));
      break;
    case LCD_IOCTL_GETBACKLIGHT:
      put_user(data->backlight ? '1' : '0', buffer);
      break;
    case LCD_IOCTL_SETBACKLIGHT:
      get_user(ch, buffer);
      lcdsetbacklight(data, (ch == '1'));
      break;
    case LCD_IOCTL_SCROLLHZ:
      get_user(ch, buffer);
      lcdscrollhoriz(data, ch - '0');
      break;
    case LCD_IOCTL_GETCUSTOMCHAR:
      get_user(ch, buffer);
      for (i=0; i<8; i++)
	put_user(data->customchars[ch][i], buffer + i + 1);
      break;
    case LCD_IOCTL_SETCUSTOMCHAR:
      for (i = 0; i < 9; i++)
	get_user(ccb[i], buffer + i);
      lcdcustomchar(data, ccb[0], ccb+1);
      break;
    case LCD_IOCTL_CLEAR:
      get_user(ch, buffer);
      if (ch == '1')
	lcdclear(data);
      break;
    default:
      printk(KERN_INFO "Unknown IOCTL\n");
      break;
  }
  CRIT_END(data);

  return status;
}

static int lcdi2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{

    data = (LcdData_t *) devm_kzalloc(&client->dev, sizeof(LcdData_t),
				      GFP_KERNEL);
    if (!data)
        return -ENOMEM;

    i2c_set_clientdata(client, data);
    sema_init(&data->sem, 1);

    data->row = 0;
    data->column = 0;
    data->handle = client;
    data->backlight = 1;
    data->cursor = cursor;
    data->blink = blink;
    data->deviceopencnt = 0;
    data->major = major;

    lcdinit(data, topo);
    lcdprint(data, "HD44780\nDriver");

    dev_info(&client->dev, "%ux%u LCD using bus 0x%X, at address 0x%X",
	     data->organization.columns,
	     data->organization.rows, busno, address);

    return 0;
}

static int lcdi2c_remove(struct i2c_client *client)
{
	LcdData_t *data = i2c_get_clientdata(client);

        dev_info(&client->dev, "going to be removed");
        if (data)
            lcdfinalize(data);

	return 0;
}

/*
 * Driver data (common to all clients)
 */

static const struct i2c_device_id lcdi2c_id[] = {
	{ "lcdi2c", 0 },
        { },
};
MODULE_DEVICE_TABLE(i2c, lcdi2c_id);

static struct i2c_driver lcdi2c_driver = {
	.driver = {
                .owner  = THIS_MODULE,
		.name	= "lcdi2c",
	},
        .probe          = lcdi2c_probe,
        .remove         = lcdi2c_remove,
	.id_table	= lcdi2c_id,
};


static struct file_operations lcdi2c_fops = {
	.read = lcdi2c_fopread,
	.write = lcdi2c_fopwrite,
	.llseek = lcdi2c_lseek,
	.unlocked_ioctl = lcdi2c_ioctl,
	.open = lcdi2c_open,
	.release = lcdi2c_release,
        .owner = THIS_MODULE,
};

static ssize_t lcdi2c_reset(struct device* dev, struct device_attribute* attr,
			    const char* buf, size_t count)
{
    CRIT_BEG(data, ERESTARTSYS);

    if (count > 0 && buf[0] == '1')
        lcdinit(data, topo);

    CRIT_END(data);
    return count;
}

static ssize_t lcdi2c_backlight(struct device* dev,
				struct device_attribute* attr,
				const char* buf, size_t count)
{
    u8 res;
    int er;

    CRIT_BEG(data, ERESTARTSYS);

    er = kstrtou8(buf, 10, &res);
    if (er == 0)
    {
        lcdsetbacklight(data,  res);
        er = count;
    }
    else if (er == -ERANGE)
        dev_err(dev, "Brightness parameter out of range (0-255).");
    else if (er == -EINVAL)
        dev_err(dev, "Brightness parameter has numerical value. \"%s\" was given", buf);
    else
        dev_err(dev, "Brightness parameter wasn't properly converted! err: %d", er);

    CRIT_END(data);
    return er;
}

static ssize_t lcdi2c_backlight_show(struct device *dev,
				     struct device_attribute *attr, char *buf)
{
    int count = 0;

    CRIT_BEG(data, ERESTARTSYS);

    if (buf)
        count = snprintf(buf, PAGE_SIZE, "%d", data->backlight);

    CRIT_END(data);
    return count;
}

static ssize_t lcdi2c_cursorpos(struct device* dev,
				struct device_attribute* attr,
				const char* buf, size_t count)
{
    CRIT_BEG(data, ERESTARTSYS);

    if (count >= 2)
    {
        count = 2;
        lcdsetcursor(data, buf[0], buf[1]);
    }

    CRIT_END(data);
    return count;
}

static ssize_t lcdi2c_cursorpos_show(struct device *dev,
				     struct device_attribute *attr,
				     char *buf)
{
    ssize_t count = 0;

    CRIT_BEG(data, ERESTARTSYS);

    if (buf)
    {
        count = 2;
        buf[0] = data->column;
        buf[1] = data->row;
    }

    CRIT_END(data);
    return count;
}

static ssize_t lcdi2c_data(struct device* dev,
			   struct device_attribute* attr,
			   const char* buf, size_t count)
{
    u8 i, addr, memaddr;

    CRIT_BEG(data, ERESTARTSYS);

    if (count > 0)
    {
	memaddr = data->column + (data->row * data->organization.columns);
	for(i = 0; i < count; i++)
	{
	  addr = (memaddr + i) % (data->organization.columns * data->organization.rows);
	  data->buffer[addr] = buf[i];
	}
	lcdflushbuffer(data);
    }

    CRIT_END(data);
    return count;
}

static ssize_t lcdi2c_data_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
    u8 i=0;

    CRIT_BEG(data, ERESTARTSYS);

    if (buf)
    {
        for (i = 0; i < (data->organization.columns * data->organization.rows); i++)
        {
            buf[i] = data->buffer[i];
        }
    }

    CRIT_END(data);
    return (data->organization.columns * data->organization.rows);
}

static ssize_t lcdi2c_meta_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{

#define META_BUFFER_LEN 54

    ssize_t count = 0;
    char tmp[12], lines[META_BUFFER_LEN];

    CRIT_BEG(data, ERESTARTSYS);

    if (buf)
    {
        int i;
        memset(lines, 0, META_BUFFER_LEN);
        for (i=0; i < data->organization.rows; i++)
        {
            snprintf(tmp, 12, "R[%d]=0x%02X ",i, data->organization.addresses[i]);
            strncat(lines, tmp, META_BUFFER_LEN);
        }

        count = snprintf(buf, PAGE_SIZE,
                         "Topology:%s=%d\n"
                         "Rows:%d\n"
                         "Columns:%d\n"
                         "Rows addresses:%s\n"
                         "Pins:RS=%d RW=%d E=%d BCKLIGHT=%d D[4]=%d D[5]=%d D[6]=%d D[7]=%d\n"
			 "IOCTLS:\n",
                         data->organization.toponame,
                         data->organization.topology,
                         data->organization.rows,
                         data->organization.columns,
                         lines,
                         PIN_RS, PIN_RW, PIN_EN,
                         PIN_BACKLIGHTON,
                         PIN_DB4, PIN_DB5, PIN_DB6, PIN_DB7
                        );

 	for(i=0; i < (sizeof(ioctls) / sizeof(IOCTLDescription_t)); i++)
 	{
 	  count += snprintf(lines, META_BUFFER_LEN, "\t%s=0x%02X\n",
                            ioctls[i].name, ioctls[i].ioctlcode);
 	  strncat(buf, lines, PAGE_SIZE);
 	}


    }

    CRIT_END(data);
    return count;
}

static ssize_t lcdi2c_cursor(struct device* dev,
			     struct device_attribute* attr,
			     const char* buf, size_t count)
{
    CRIT_BEG(data, ERESTARTSYS);

    if (count > 0)
    {
        data->cursor = (buf[0] == '1');
        lcdcursor(data, data->cursor);
    }

    CRIT_END(data);
    return count;
}

static ssize_t lcdi2c_cursor_show(struct device *dev,
				  struct device_attribute *attr, char *buf)
{
    int count = 0;

    CRIT_BEG(data, ERESTARTSYS);

    if (buf)
        count = snprintf(buf, PAGE_SIZE, "%c", data->cursor ? '1' : '0');

    CRIT_END(data);
    return count;
}

static ssize_t lcdi2c_blink(struct device* dev,
			    struct device_attribute* attr,
			    const char* buf, size_t count)
{
    CRIT_BEG(data, ERESTARTSYS);

    if (count > 0)
    {
        data->blink = (buf[0] == '1');
        lcdblink(data, data->blink);
    }

    CRIT_END(data);
    return count;
}

static ssize_t lcdi2c_blink_show(struct device *dev,
				 struct device_attribute *attr, char *buf)
{
    int count = 0;

    CRIT_BEG(data, ERESTARTSYS);

    if (buf)
        count = snprintf(buf, PAGE_SIZE, "%c", data->blink ? '1' : '0');

    CRIT_END(data);
    return count;
}

static ssize_t lcdi2c_home(struct device* dev, struct device_attribute* attr,
			   const char* buf, size_t count)
{
    CRIT_BEG(data, ERESTARTSYS);

    if (count > 0 && buf[0] == '1')
        lcdhome(data);

    CRIT_END(data);
    return count;
}

static ssize_t lcdi2c_clear(struct device* dev, struct device_attribute* attr,
			    const char* buf, size_t count)
{
    CRIT_BEG(data, ERESTARTSYS);

    if (count > 0 && buf[0] == '1')
        lcdclear(data);

    CRIT_END(data);
    return count;
}

static ssize_t lcdi2c_scrollhz(struct device* dev, struct device_attribute* attr,
			       const char* buf, size_t count)
{
    CRIT_BEG(data, ERESTARTSYS);

    if (count > 0)
        lcdscrollhoriz(data, buf[0] - '0');

    CRIT_END(data);
    return count;
}

static ssize_t lcdi2c_customchar(struct device* dev,
				 struct device_attribute* attr,
				 const char* buf, size_t count)
{
    u8 i;

    CRIT_BEG(data, ERESTARTSYS);

    if ((count > 0 && (count % 9)) || count == 0)
    {
         dev_err(&client->dev, "incomplete character bitmap definition\n");
	 CRIT_END(data);
         return -ETOOSMALL;
    }

    for (i = 0; i < count; i+=9)
    {
        if (buf[i] > 7)
        {
            dev_err(&client->dev, "character number %d can only have values"
				  "starting from 0 to 7\n", buf[i]);
	    CRIT_END(data);
	    return -ETOOSMALL;
        }

        lcdcustomchar(data, buf[i], buf + i + 1);
    }

    CRIT_END(data);
    return count;
}

static ssize_t lcdi2c_customchar_show(struct device *dev,
				      struct device_attribute *attr, char *buf)
{
    int count = 0, c, i;

    CRIT_BEG(data, ERESTARTSYS);

    for (c = 0; c < 8; c++)
    {
        buf[c * 9] = c;
	count++;
        for (i = 0; i < 8; i++)
        {
            buf[c * 9 + (i + 1)] = data->customchars[c][i];
            count++;
        }
    }

    CRIT_END(data);
    return count;
}

static ssize_t lcdi2c_char(struct device* dev,
				 struct device_attribute* attr,
				 const char* buf, size_t count)
{
    u8 memaddr;

    CRIT_BEG(data, ERESTARTSYS);

    if (buf && count > 0)
    {
      memaddr = (1 + data->column + (data->row * data->organization.columns))
                % LCD_BUFFER_SIZE;
      lcdwrite(data, buf[0]);
      data->column = (memaddr % data->organization.columns);
      data->row = (memaddr / data->organization.columns);
      lcdsetcursor(data, data->column, data->row);
    }

    CRIT_END(data);
    return 1;
}

static ssize_t lcdi2c_char_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
    u8 memaddr;

    CRIT_BEG(data, ERESTARTSYS);

    memaddr = (data->column + (data->row * data->organization.columns))
              % LCD_BUFFER_SIZE;
    buf[0] = data->buffer[memaddr];

    CRIT_END(data);
    return 1;
}

DEVICE_ATTR(reset, S_IWUSR | S_IWGRP, NULL, lcdi2c_reset);

/* This attribute will be replaced by "brightness" attr. Until now, it just
 * duplicate brightness
 */
DEVICE_ATTR(brightness, S_IWUSR | S_IWGRP | S_IRUSR | S_IRGRP | S_IROTH,
	    lcdi2c_backlight_show, lcdi2c_backlight);
DEVICE_ATTR(position, S_IWUSR | S_IWGRP | S_IRUSR | S_IRGRP | S_IROTH,
	    lcdi2c_cursorpos_show, lcdi2c_cursorpos);
DEVICE_ATTR(data, S_IWUSR | S_IWGRP | S_IRUSR | S_IRGRP | S_IROTH,
	    lcdi2c_data_show, lcdi2c_data);
DEVICE_ATTR(meta, S_IRUSR | S_IRGRP, lcdi2c_meta_show, NULL);
DEVICE_ATTR(cursor, S_IWUSR | S_IWGRP | S_IRUSR | S_IRGRP | S_IROTH,
	    lcdi2c_cursor_show, lcdi2c_cursor);
DEVICE_ATTR(blink, S_IWUSR | S_IWGRP | S_IRUSR | S_IRGRP | S_IROTH,
	    lcdi2c_blink_show, lcdi2c_blink);
DEVICE_ATTR(home, S_IWUSR | S_IWGRP, NULL, lcdi2c_home);
DEVICE_ATTR(clear, S_IWUSR | S_IWGRP, NULL, lcdi2c_clear);
DEVICE_ATTR(scrollhz, S_IWUSR | S_IWGRP, NULL, lcdi2c_scrollhz);
DEVICE_ATTR(customchar, S_IWUSR | S_IWGRP | S_IRUSR | S_IRGRP | S_IROTH,
	    lcdi2c_customchar_show, lcdi2c_customchar);
DEVICE_ATTR(character, S_IWUSR | S_IWGRP | S_IRUSR | S_IRGRP | S_IROTH,
	    lcdi2c_char_show, lcdi2c_char );

static const struct attribute *i2clcd_attrs[] = {
	&dev_attr_reset.attr,
        &dev_attr_brightness.attr,
	&dev_attr_position.attr,
	&dev_attr_data.attr,
	&dev_attr_meta.attr,
        &dev_attr_cursor.attr,
        &dev_attr_blink.attr,
        &dev_attr_home.attr,
        &dev_attr_clear.attr,
        &dev_attr_scrollhz.attr,
        &dev_attr_customchar.attr,
	&dev_attr_character.attr,
	NULL,
};

static const struct attribute_group i2clcd_device_attr_group = {
	.attrs = (struct attribute **) i2clcd_attrs,
};

static int __init i2clcd857_init(void)
{
    int ret;
    struct i2c_board_info board_info = {
                    .type = "lcdi2c",
                    .addr = address,

    };

    adapter = i2c_get_adapter(busno);
    if (!adapter) return -EINVAL;

     client = i2c_new_device(adapter, &board_info);
     if (!client) return -EINVAL;

    ret = i2c_add_driver(&lcdi2c_driver);
    if (ret)
    {
        i2c_put_adapter(adapter);
        return ret;
    }

    if (!i2c_check_functionality(adapter, I2C_FUNC_I2C))
    {
        i2c_put_adapter(adapter);
        dev_err(&client->dev, "no algorithms associated to i2c bus\n");
        return -ENODEV;
    }

    i2c_put_adapter(adapter);

    major = register_chrdev(major, DEVICE_NAME, &lcdi2c_fops);
    if (major < 0)
    {
        dev_warn(&client->dev, "failed to register device with error: %d\n",
		 major);
        goto failed_chrdev;
    }

    lcdi2c_class = class_create(THIS_MODULE, DEVICE_CLASS_NAME);
    if (IS_ERR(lcdi2c_class))
    {
        dev_warn(&client->dev, "class creation failed %s\n", DEVICE_CLASS_NAME);
        goto failed_class;
    }

    lcdi2c_device = device_create(lcdi2c_class, NULL, MKDEV(major, minor), NULL,
				  DEVICE_NAME);
    if (IS_ERR(lcdi2c_device))
    {
        dev_warn(&client->dev, "device %s creation failed\n", DEVICE_NAME);
        goto failed_device;
    }

    if(sysfs_create_group(&lcdi2c_device->kobj, &i2clcd_device_attr_group))
    {
        dev_warn(&client->dev, "device attribute group creation failed\n");
        goto failed_device;
    }

     dev_info(&client->dev, "registered with major %u\n", major);


    return ret;

    failed_device:
        class_unregister(lcdi2c_class);
        class_destroy(lcdi2c_class);
    failed_class:
        unregister_chrdev(major, DEVICE_NAME);
    failed_chrdev:
        i2c_unregister_device(client);
        i2c_del_driver(&lcdi2c_driver);
    return -1;
}


static void __exit i2clcd857_exit(void)
{

    unregister_chrdev(major, DEVICE_NAME);

    sysfs_remove_group(&lcdi2c_device->kobj, &i2clcd_device_attr_group);
    device_destroy(lcdi2c_class, MKDEV(major, 0));
    class_unregister(lcdi2c_class);
    class_destroy(lcdi2c_class);
    unregister_chrdev(major, DEVICE_NAME);

     if (client)
         i2c_unregister_device(client);

    i2c_del_driver(&lcdi2c_driver);
}


/*
  LCD topology description, first four bytes defines subsequent row of text addresses and how
  are they mapped in internal RAM of LCD, last two bytes describes number of columns and number
  of rows.
  LCD_TOPO_16x1T1 is same as 8x2. RAM is divided in two areas by 8 bytes, left bytes
  are starting from address 0x00, right hand half of LCD is starting from 0x40, that's
  exactly how 8x2 LCD is organized. So this type of an LCD can be considered as 8x2 LCD with two
  rows laying in the same line on both halfs of an LCD, instead both lines stacked on the top
  of each other.
  Type 2 of 16x1 has straighforward organization, first sixteen bytes representing a row on an LCD.
*/
static const u8 topoaddr[][6] ={{0x00, 0x40, 0x00, 0x00, 40, 2}, //LCD_TOPO_40x2
                                     {0x00, 0x40, 0x14, 0x54, 20, 4}, //LCD_TOPO_20x4
                                     {0x00, 0x40, 0x00, 0x00, 20, 2}, //LCD_TOPO_20x2
                                     {0x00, 0x40, 0x10, 0x50, 16, 4}, //LCD_TOPO_16x4
                                     {0x00, 0x40, 0x00, 0x00, 16, 2}, //LCD_TOPO_16x2
                                     {0x00, 0x40, 0x00, 0x40, 8,  2}, //LCD_TOPO_16x1T1
                                     {0x00, 0x08, 0x00, 0x08, 16, 1}, //LCD_TOPO_16x1T2
				     {0x00, 0x40, 0x00, 0x40, 8,  2}, //LCD_TOPO_8x2
                                    };

/* Text representation of various LCD topologies */
static const char *toponames[] = {
                                    "40x2",
                                    "20x4",
                                    "20x2",
                                    "16x4",
                                    "16x2",
                                    "16x1 type 1",
                                    "16x1 type 2",
				    "8x2",
                                    };

/* Pin mapping array */
uint pinout[8] = {0,1,2,3,4,5,6,7}; //I2C module pinout configuration in order:
                                    //RS,RW,E,BL,D4,D5,D6,D7

void _udelay_(u32 usecs)
{
    udelay(usecs);
}

/**
 * write a byte to i2c device, sets backlight pin
 * on or off depending on current stup in LcdData struture
 * given as parameter.
 *
 * @param LcdData_t* lcd handler structure address
 * @param u8 byte to send to LCD
 * @return none
 *
 */
static void _buswrite(LcdData_t *lcd, u8 data)
{
    data |= lcd->backlight ? (1 << PIN_BACKLIGHTON) : 0;
    LOWLEVEL_WRITE(lcd->handle, data);
}

/**
 * write a byte to i2c device, strobing EN pin of LCD
 *
 * @param LcdData_t* lcd handler structure address
 * @param u8 byte to send to LCD
 * @return none
 *
 */
static void _strobe(LcdData_t *lcd, u8 data)
{
    _buswrite(lcd, data | (1 << PIN_EN));
    USLEEP(1);
    _buswrite(lcd, data & (~(1 << PIN_EN)));
    USLEEP(50);
}

/**
 * write a byte using 4 bit interface
 *
 * @param LcdData_t* lcd handler structure address
 * @param u8 byte to send to LCD
 * @return none
 *
 */
static void _write4bits(LcdData_t *lcd, u8 value)
{
    _buswrite(lcd, value);
    _strobe(lcd, value);
}

/**
 * write a byte to a LCD splitting byte into two nibbles
 * of 4 bits each.
 *
 * @param LcdData_t* lcd handler structure address
 * @param u8 byte to send to LCD
 * @param u8 mode of communication (RS line of a LCD)
 * @return none
 *
 */
static void lcdsend(LcdData_t *lcd, u8 value, u8 mode)
{
    u8 highnib = value & 0xF0;
    u8 lownib = value << 4;

    _write4bits(lcd, (highnib) | mode);
    _write4bits(lcd, (lownib) | mode);
}

/**
 * copy buffer of buffer from host to LCD
 *
 * @param LcdData_t* lcd handler structure address
 * @return none
 *
 */
void lcdflushbuffer(LcdData_t *lcd)
{
    u8 col = lcd->column, row = lcd->row, i;

    for(i = 0; i < (lcd->organization.columns * lcd->organization.rows); i++)
    {
        lcdcommand(lcd, LCD_DDRAM_SET + ITOMEMADDR(lcd, i));
        lcdsend(lcd, lcd->buffer[i], (1 << PIN_RS));
    }
    lcdsetcursor(lcd, col, row);
}

/**
 * send command to a LCD.
 *
 * @param LcdData_t* lcd handler structure address
 * @param u8 command byte to send
 * @return none
 *
 */
void lcdcommand(LcdData_t *lcd, u8 data)
{
    lcdsend(lcd, data, 0);
}

/**
 * write byte of data to LCD
 *
 * @param LcdData_t* lcd handler structure address
 * @param u8 data byte to send
 * @return none
 *
 */
void lcdwrite(LcdData_t *lcd, u8 data)
{
    u8 memaddr;

    memaddr = (lcd->column + (lcd->row * lcd->organization.columns)) % LCD_BUFFER_SIZE;
    lcd->buffer[memaddr] = data;

    lcdsend(lcd, data, (1 << PIN_RS));
}

/**
 * set cursor to given position. Row number and column number
 * depends on current LCD topology, if values are to high
 * then are wrappped around and set to 0
 *
 *
 * @param LcdData_t* lcd handler structure address
 * @param u8 column number
 * @param u8 row number
 * @return none
 *
 */
void lcdsetcursor(LcdData_t *lcd, u8 column, u8 row)
{
     lcd->column = (column >= lcd->organization.columns ? 0 : column);
     lcd->row = (row >= lcd->organization.rows ? 0 : row);
    lcdcommand(lcd, LCD_DDRAM_SET | PTOMEMADDR(lcd, lcd->column, lcd->row));
}

/**
 * switches backlight on or off.
 *
 * @param LcdData_t* lcd handler structure address
 * @param u8 false value switechs backlight off, otherwise backlight will
 *                be switched on
 * @return none
 *
 */
void lcdsetbacklight(LcdData_t *lcd, u8 backlight)
{
    lcd->backlight = backlight;
    _buswrite(lcd, lcd->backlight ? (1 << PIN_BACKLIGHTON) : 0);
}

/**
 * switches visibility of cursor on or off
 *
 * @param LcdData_t* lcd handler structure address
 * @param u8 false will switch it off, otherwise it will be switched on
 * @return none
 *
 */
void lcdcursor(LcdData_t *lcd, u8 cursor)
{
    if (cursor)
        lcd->displaycontrol |= LCD_CURSOR;
    else
        lcd->displaycontrol &= ~LCD_CURSOR;

    lcdcommand(lcd, lcd->displaycontrol);
}

/**
 * switch blink of a cursor on or of
 *
 * @param LcdData_t* lcd handler structure address
 * @param u8 false will switch it off, otherwise it will be switched on
 * @return none
 *
 */
void lcdblink(LcdData_t *lcd, u8 blink)
{
    if (blink)
        lcd->displaycontrol |= LCD_BLINK;
    else
        lcd->displaycontrol &= ~LCD_BLINK;

    lcdcommand(lcd, lcd->displaycontrol);
}

/**
 * will set LCD back to home, which usually is cursor set at position 0,0
 *
 * @param LcdData_t* lcd handler structure address
 * @return none
 *
 */
void lcdhome(LcdData_t *lcd)
{
    lcd->column = 0;
    lcd->row = 0;
    lcdcommand(lcd, LCD_HOME);
    MSLEEP(2);
}

/**
 * clears buffer by setting all bytes to 0x20 (ASCII code for space) and
 * send clear command to a LCD
 *
 * @param LcdData_t* lcd handler structure address
 * @return none
 *
 */
void lcdclear(LcdData_t *lcd)
{
    memset(lcd->buffer, 0x20, LCD_BUFFER_SIZE); //Fill buffer with spaces
    lcdcommand(lcd, LCD_CLEAR);
    MSLEEP(2);
}

/**
 * scrolls content of a LCD horizontally. This is internal feature of HD44780
 * it scrolls content without actually changing content of internal RAM, so
 * buffer of host stays intact as well
 *
 * @param LcdData_t* lcd handler structure address
 * @param u8 direction of a scroll, true - right, false - left
 * @return none
 *
 */
void lcdscrollhoriz(LcdData_t *lcd, u8 direction)
{
    lcdcommand(lcd, LCD_DS_SHIFTDISPLAY |
	      (direction ? LCD_DS_SHIFTRIGHT : LCD_DS_SHIFTLEFT));
}

/**
 * TODO - not implemented yet
 *
 * @param LcdData_t* lcd handler structure address
 *
 */
void lcdscrollvert(LcdData_t *lcd, u8 direction)
{
    //TODO: Vertical scroll
}

/**
 * prints C string data on LCD, this function is doing some simple
 * interpretation of some special characters in string, like \n \r or
 * backspace. Every carriage return or return character will move cursor
 * to line below current one and backspace character will move cursor to the
 * left until it reaches first character of the line. If string is longer, than
 * what LCD is capable to display, cursor wraps around and print will overwrite
 * existing text.
 *
 * @param LcdData_t* lcd handler structure address
 * @param char* data 0 terinated string
 * @return u8 bytes written
 *
 */
u8 lcdprint(LcdData_t *lcd, const char *data)
{
    int i = 0;

    while (i < (lcd->organization.columns * lcd->organization.rows) && data[i])
    {
      if (data[i] == '\n' || data[i] == '\r')
      {
	if (lcd->organization.rows > 1) //For one-liners we do not reset column
	                                //counter
	  lcd->column = 0;
	lcd->row = (lcd->row + 1) % lcd->organization.rows;
	i++;
	continue;
      } else if (data[i] == 0x08) //BS
      {
	if (lcd->column > 0)
	  lcd->column -= 1;
	i++;
	continue;
      }

      lcdcommand(lcd, LCD_DDRAM_SET | PTOMEMADDR(lcd, lcd->column, lcd->row));
      lcdwrite(lcd, data[i]);
      i++;
      lcd->column = (lcd->column + 1) % lcd->organization.columns;
      if (lcd->column == 0)
	lcd->row = (lcd->row + 1) % lcd->organization.rows;
    }

    return (lcd->column + (lcd->row * lcd->organization.columns));
}

/**
 * alows to define custom character. It is feature of HD44780 controller.
 *
 * @param LcdData_t* lcd handler structure address
 * @param u8 character number to define 0-7
 * @param u8* array of 8 bytes of bitmap definition
 * @return none
 *
 */
void lcdcustomchar(LcdData_t *lcd, u8 num, const u8 *bitmap)
{
    u8 i;

    num &= 0x07;
    lcdcommand(lcd, LCD_CGRAM_SET | (num << 3));

    for(i = 0; i < 8; i++)
    {
        lcd->customchars[num][i] = bitmap[i];
	lcdsend(lcd, bitmap[i], (1 << PIN_RS));
    }
}

/**
 * LCD deinitialization procedure
 *
 * @param LcdData_t* lcd handler structure address
 * @return none
 *
 */
void lcdfinalize(LcdData_t *lcd)
{
    lcdsetbacklight(lcd, 0);
    lcdclear(lcd);
    lcdclear(lcd);
    lcdcommand(lcd, LCD_DC_DISPLAYOFF | LCD_DC_CURSOROFF | LCD_DC_CURSORBLINKOFF);
}

/**
 * initialization procedure of LCD
 *
 * @param LcdData_t* lcd handler structure address
 * @param lcd_topology number representing topology of LCD
 * @return none
 *
 */
void lcdinit(LcdData_t *lcd, lcd_topology topo)
{
    memset(lcd->buffer, 0x20, LCD_BUFFER_SIZE); //Fill buffer with spaces

    if (topo > LCD_TOPO_8x2)
        topo = LCD_TOPO_16x2;

    lcd->organization.topology = topo;
    lcd->organization.columns = topoaddr[topo][4];
    lcd->organization.rows = topoaddr[topo][5];
    memcpy(lcd->organization.addresses, topoaddr[topo], sizeof(topoaddr[topo]) - 2);
    lcd->organization.toponame = toponames[topo];

    lcd->displaycontrol = 0;
    lcd->displaymode = 0;

    lcd->displayfunction = LCD_FS_4BITDATA | LCD_FS_1LINE | LCD_FS_5x8FONT;
    if (lcd->organization.rows > 1)
        lcd->displayfunction |= LCD_FS_2LINES;

    MSLEEP(50);
    _buswrite(lcd, lcd->backlight ? (1 << PIN_BACKLIGHTON) : 0);
    MSLEEP(100);

    _write4bits(lcd, (1 << PIN_DB4) | (1 << PIN_DB5));
    MSLEEP(5);

    _write4bits(lcd, (1 << PIN_DB4) | (1 << PIN_DB5));
    MSLEEP(5);

    _write4bits(lcd, (1 << PIN_DB4) | (1 << PIN_DB5));
    MSLEEP(15);

    _write4bits(lcd, (1 << PIN_DB5));

    lcdcommand(lcd, lcd->displayfunction);

    lcd->displaycontrol |= LCD_DC_DISPLAYON | LCD_DC_CURSOROFF |
			   LCD_DC_CURSORBLINKOFF;
    lcdcommand(lcd, lcd->displaycontrol);

    lcdclear(lcd);
    lcdhome(lcd);

    lcdsetcursor(lcd, lcd->column, lcd->row);
    lcdcursor(lcd, lcd->cursor);
    lcdblink(lcd, lcd->blink);
}


module_init(i2clcd857_init);
module_exit(i2clcd857_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jarek Zok <jarekzok@gmail.com>");
MODULE_DESCRIPTION("Driver for HD44780 LCD with PCF8574 I2C extension.");
MODULE_VERSION("0.1.0");
