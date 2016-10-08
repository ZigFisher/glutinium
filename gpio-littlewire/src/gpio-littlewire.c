/*
 * Little Wire - Little Wire USB device GPIO driver
 * Copyright (C) 2013 David Lynch, DLA Systems
 *
 * tested on Olimexino-85
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mutex.h>
#include <linux/slab.h>

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/types.h>

/* include interfaces to usb layer */
#include <linux/usb.h>
#include <linux/gpio.h>


/**
 * enum         Little Wire Commands
 */
enum {
       /* Generic requests */
       LW_ECHO,        /* 00 echo test */
       LW_READ,        /* 01 read byte (wIndex:address) */
       LW_WRITE,       /* 02 write byte (wIndex:address, wValue:value)*/
       LW_CLR,         /* 03 clear bit (wIndex:address, wValue:bitno) */
       LW_SET,         /* 04 set bit (wIndex:address, wValue:bitno) */
       /* Programming requests */
       LW_POWERUP,     /* 05 apply power (wValue:SCK-period, wIndex:RESET) */
       LW_POWERDOWN,   /* 06 remove power from chip */
       LW_SPI,         /* 07 issue SPI command (wValue:c1c0, wIndex:c3c2) */
       LW_POLL_BYTES,  /* 08 set poll bytes for write (wValue:p1p2) */
       LW_FLASH_READ,  /* 09 read flash (wIndex:address) */
       LW_FLASH_WRITE, /* 10 write flash (wIndex:address, wValue:timeout) */
       LW_EEPROM_READ, /* 11 read eeprom (wIndex:address) */
       LW_EEPROM_WRITE, /* 12 write eeprom (wIndex:address, wValue:timeout) */
    /* Additional requests - ihsanKehribar */

       LW_PIN_SET_INPUT,           /* 13 */
       LW_PIN_SET_OUTPUT,          /* 14 */
       LW_READ_ADC,            /* 15 */
       LW_SETUP_PWM,           /* 16 */
       LW_UPDATE_PWM_COMPARE,  /* 17 */
       LW_PIN_SET_HIGH,            /* 18 */
       LW_PIN_SET_LOW,         /* 19 */
       LW_PIN_READ,            /* 20 */
       LW_SINGLE_SPI,          /* 21 */
       LW_CHANGE_PWM_PRESCALE, /* 22 */
       LW_SETUP_SPI,           /* 23 */
       LW_SETUP_I2C,           /* 24 */
       LW_I2C_BEGIN_TX,            /* 25 */
       LW_I2C_ADD_BUFFER,          /* 26 */
       LW_I2C_SEND_BUFFER,         /* 27 */
       LW_SPI_ADD_BUFFER,          /* 28 */
       LW_SPI_SEND_BUFFER,         /* 29 */
       LW_I2C_REQUEST_FROM,    /* 30 */
       LW_SPI_UPDATE_DELAY,    /* 31 */
       LW_STOP_PWM,            /* 32 */
       LW_DEBUG_SPI,           /* 33 */
       LW_VERSION_QUERY,           /* 34 */
};

#define USB_TIMEOUT         2000
#define GEN_LW_NUMBER_GPIOS    5
#define LW_USB_TYPE_OUT     0x40
#define LW_USB_TYPE_IN      0xc0
/*
 * GPIO names
 */
static const char *lw_gpio_names[GEN_LW_NUMBER_GPIOS] = {
       "PB0", "PB1", "PB2", "PB3", "PB5"
};


/**
 * struct lw_gpio - local driver data for Little Wire device
 * @usb_dev:        pointer to the USB device
 * @interface:      pointer to the USB interface
 * @gpio_chip:      pointer to the gpio lib struct
 * @lock:           mutex for the device
 */
struct lw_gpio {
       struct usb_device *usb_dev;     /* the usb device for this device */
       struct usb_interface *interface;/* the interface for this device */
       //struct device *master;
       //u8 *buffer;
       struct gpio_chip gpio_chip;
       struct mutex lock;
};

/**
 * gpio_to_lw() - derive local information pointer from global information pointer
 * @gc:         global information pointer.
 *
 * device a pointer to the local struct lw_gpio from a pointer to the 
 * global struct gpio_chip
 *
 * Return: pointer to the local lw_gpio struct.
 */

static struct lw_gpio *
gpio_to_lw(struct gpio_chip *gc)
{
       return container_of(gc, struct lw_gpio, gpio_chip);
}

/**
 * lw_usb_read() - submit a Little Wire USB read request
 * @gc:         global information pointer.
 * @pin:        gpio pin to read.
 * @cmd:        Little Wire command to read.
 *
 * Return: first byte of data read.
 */
static int
lw_usb_read(struct gpio_chip *gc, unsigned pin, unsigned cmd)
{
       struct lw_gpio *lwgc = gpio_to_lw(gc);
       uint8_t data[8];
       int rc = 0;

       mutex_lock(&lwgc->lock);
       rc = usb_control_msg(lwgc->usb_dev, usb_rcvctrlpipe(lwgc->usb_dev, 0),
                            cmd, LW_USB_TYPE_IN, pin, 0, data, sizeof(data),
                            USB_TIMEOUT);
       mutex_unlock(&lwgc->lock);
       if (rc != sizeof(data))
               dev_err(&lwgc->interface->dev, "lw_usb_read error %d\n", rc);
       dev_info(&lwgc->interface->dev, "lw_usb_read pin %d = %d\n", pin,
                data[0]);

       return data[0];
}

/**
 * lw_usb_write() - submit a Little Wire USB write request
 * @gc:         global information pointer.
 * @pin:        gpio pin to write.
 * @cmd:        Little Wire command to write.
 *
 * Return: first byte of data read.
 */
static int
lw_usb_write(struct gpio_chip *gc, unsigned pin, unsigned cmd)
{
       struct lw_gpio *lwgc = gpio_to_lw(gc);
       uint8_t data[8];
       int rc = 0;

       mutex_lock(&lwgc->lock);
       rc = usb_control_msg(lwgc->usb_dev, usb_sndctrlpipe(lwgc->usb_dev, 0),
                            cmd, LW_USB_TYPE_OUT, pin, 0, data, sizeof(data),
                            USB_TIMEOUT);
       mutex_unlock(&lwgc->lock);
       if (rc != sizeof(data))
               dev_err(&lwgc->interface->dev, "lw_usb_write error %d\n", rc);
       dev_info(&lwgc->interface->dev, "lw_usb_write pin %d = %d\n", pin,
                data[0]);

       return data[0];
}

/**
 * lw_get_value() - read a gpio pin value
 * @gc:         global information pointer.
 * @pin:        gpio pin to read.
 *
 * Return: pin state
 */
static int
lw_get_value(struct gpio_chip *gc, unsigned pin)
{
       return lw_usb_read(gc, pin, LW_PIN_READ);
}

/**
 * lw_set_value() - write a gpio pin value
 * @gc:         global information pointer.
 * @pin:        gpio pin to write.
 *
 * Return: none
 */
static void
lw_set_value(struct gpio_chip *gc, unsigned pin, int state)
{
       int rc;

       if (state) {
               rc = lw_usb_write(gc, pin, LW_PIN_SET_HIGH);
               return;
       }
       rc = lw_usb_write(gc, pin, LW_PIN_SET_LOW);
       return;
}

/**
 * lw_direction_input() - set pin to input
 * @gc:         global information pointer.
 * @pin:        gpio pin to make input.
 *
 * Return: success
 */
static int
lw_direction_input(struct gpio_chip *gc, unsigned pin)
{
       lw_usb_write(gc, pin, LW_PIN_SET_INPUT);
       return 0;
}

/**
 * lw_direction_output() - set pin to output
 * @gc:         global information pointer.
 * @pin:        gpio pin to make output.
 *
 * Return: success
 */
static int
lw_direction_output(struct gpio_chip *gc, unsigned pin, int val)
{
       lw_usb_write(gc, pin, LW_PIN_SET_OUTPUT);
       return 0;
}

/* ----- begin of usb layer ---------------------------------------------- */

/*
 * Initially the usb interface uses a vid/pid pair donated by
 * Future Technology Devices International Ltd., later a pair was
 * bought from EZPrototypes
 */
static const struct usb_device_id lw_usb_table[] = {
       {USB_DEVICE(0x1781, 0x0c9f)},   /* USB ATTINY85 */
       {}                      /* Terminating entry */
};

MODULE_DEVICE_TABLE(usb, lw_usb_table);

/**
 * lw_usb_free() - release the USB device
 * @lwgc:         local information pointer.
 *
 * Return: none
 */
static void
lw_usb_free(struct lw_gpio *lwgc)
{
       usb_put_dev(lwgc->usb_dev);
       kfree(lwgc);
}

/**
 * lw_usb_probe() - check for device presence and if so setup driver
 * @interface:      pointer to usb interface to check
 * @id:             usb id to match
 *
 * Return: 0 on SUCCESS
 */
static int
lw_usb_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
       struct lw_gpio *lwgc;
       int rc = -ENOMEM;
       u16 version;

       dev_dbg(&interface->dev, "probing usb device\n");

       /* allocate memory for our device state and initialize it */
       lwgc = kzalloc(sizeof(struct lw_gpio), GFP_KERNEL);
       if (lwgc == NULL) {
               dev_err(&interface->dev, "Out of memory\n");
               goto error;
       }

       lwgc->usb_dev = usb_get_dev(interface_to_usbdev(interface));
       lwgc->interface = interface;

       /* save our data pointer in this interface device */
       usb_set_intfdata(interface, lwgc);

       version = le16_to_cpu(lwgc->usb_dev->descriptor.bcdDevice);
       dev_info(&interface->dev,
                "version %x.%02x found at bus %03d address %03d\n",
                version >> 8, version & 0xff,
                lwgc->usb_dev->bus->busnum, lwgc->usb_dev->devnum);


       mutex_init(&lwgc->lock);
       lwgc->gpio_chip.direction_input = lw_direction_input;
       lwgc->gpio_chip.direction_output = lw_direction_output;
       lwgc->gpio_chip.get = lw_get_value;
       lwgc->gpio_chip.set = lw_set_value;
       lwgc->gpio_chip.ngpio = GEN_LW_NUMBER_GPIOS;
       lwgc->gpio_chip.names = lw_gpio_names, lwgc->gpio_chip.can_sleep = 1;
       lwgc->gpio_chip.owner = THIS_MODULE;
//       rc = gpiochip_add(&lwgc->gpio_chip);

       if (rc) {
               dev_err(&interface->dev, "Failed writing: %d\n", rc);
               goto error;
       }

       /* this should be get little wire version */
       rc = lw_usb_read(&lwgc->gpio_chip, 0, 34);
       dev_info(&interface->dev, "LittleWire %d.%02d\n", (rc >> 4) & 0xf,
                rc & 0xf);
       dev_info(&interface->dev, "LittleWire %x\n", rc);

       /* inform user about successful attachment to gpio layer */
       dev_info(&interface->dev, "connected gpio-lw-usb device\n");

       return 0;

 error:
       if (lwgc) {
               mutex_destroy(&lwgc->lock);
               lw_usb_free(lwgc);
       }
       return rc;
}

/**
 * lw_usb_disconnect() - disconnect driver from USB
 * @interface:        USB interface being disconnected from
 *
 * Return: none
 */
static void
lw_usb_disconnect(struct usb_interface *interface)
{
       struct lw_gpio *lwgc = usb_get_intfdata(interface);
       int rc;

       if (lwgc == NULL)
               return;

//       rc = gpiochip_remove(&lwgc->gpio_chip);
       if (!rc)
               mutex_destroy(&lwgc->lock);
       else
               dev_err(&interface->dev,
                       "Failed to remove the GPIO controller: %d\n", rc);

       usb_set_intfdata(interface, NULL);
       lw_usb_free(lwgc);

       dev_dbg(&interface->dev, "disconnected\n");
}

static struct usb_driver lw_usb_driver = {
       .name = "gpio-lw-usb",
       .probe = lw_usb_probe,
       .disconnect = lw_usb_disconnect,
       .id_table = lw_usb_table,
};

module_usb_driver(lw_usb_driver);
/* ----- end of usb layer ------------------------------------------------ */

MODULE_AUTHOR("David Lynch");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("GPIO driver for Little Wire");
