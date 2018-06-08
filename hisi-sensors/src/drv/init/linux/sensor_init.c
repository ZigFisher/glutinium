#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/version.h>
#include <linux/of_platform.h>

#include "sensor.h"

#define CLK_DEFAULT_FREQ 37125000  //37.125M

char * sensor_bus_type = {"i2c"};
module_param(sensor_bus_type, charp, S_IRUGO);

unsigned int sensor_bus_dev = 0;
module_param(sensor_bus_dev, uint, S_IRUGO);

unsigned int sensor_clk_frequency = CLK_DEFAULT_FREQ;
module_param(sensor_clk_frequency, uint, S_IRUGO);

char * sensor_pinmux_mode = {"0"};
module_param(sensor_pinmux_mode, charp, S_IRUGO);

struct sensor_node_dev {
	struct device *dev;
	struct clk *clk;
	unsigned int init_clk_freq;
	unsigned int clk_freq;
	struct pinctrl *p;
	struct pinctrl_state *state;
	struct sensor_device sensor_dev;
};

static int sensor_hw_init(struct platform_device *pdev)
{
	struct sensor_node_dev *node_dev;
	
	node_dev = platform_get_drvdata(pdev);
	
	clk_prepare_enable(node_dev->clk);
	clk_set_rate(node_dev->clk, node_dev->clk_freq);
	
	return 0;
}

static void sensor_hw_exit(struct platform_device *pdev)
{
	struct sensor_node_dev *node_dev;
	
	node_dev = platform_get_drvdata(pdev);

	clk_disable_unprepare(node_dev->clk);
}

#ifdef CONFIG_HISI_SNAPSHOT_BOOT
static int hi35xx_sensor_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct sensor_node_dev *node_dev;
	
	node_dev = platform_get_drvdata(pdev);
	
	clk_set_rate(node_dev->clk, node_dev->init_clk_freq);
	sensor_hw_exit(pdev);
	pinctrl_get_select(&pdev->dev, "sleep");

	return 0;
}

static int hi35xx_sensor_resume(struct platform_device *pdev)
{
	struct sensor_node_dev *node_dev;

	node_dev = platform_get_drvdata(pdev);
	if(node_dev->p && node_dev->state)
	{
		if (pinctrl_select_state(node_dev->p, node_dev->state)) 
		{
	        dev_err(&pdev->dev, "failed to activate %s pinctrl state\n", sensor_pinmux_mode);
	    }
	}
	
	sensor_hw_init(pdev);
	return 0;
}
#endif

static int is_sensor_pinmux_mode(char * pinmux_mode)
{
	if (!strncmp(pinmux_mode, "i2c_mipi", 10)
		|| !strncmp(pinmux_mode, "ssp_mipi", 10)
		|| !strncmp(pinmux_mode, "i2c_dc", 10)
		|| !strncmp(pinmux_mode, "ssp_dc", 10))
	{
		return HI_TRUE;
	}

	return HI_FALSE;
}

static int sensor_probe_of(struct sensor_node_dev *node_dev)
{
	struct device_node *np = node_dev->dev->of_node;
	struct device *dev;
	struct sensor_device *sensor_dev;
	const char *string = NULL;
	int ret;

	if (!np)
		return -ENODEV;

    //printk("np name is %s.\n", np->name);
    dev = node_dev->dev;

	sensor_dev = &node_dev->sensor_dev;
	sensor_dev->s32SensorIndex = of_alias_get_id(np, "sensor");
	if (sensor_dev->s32SensorIndex < 0 || sensor_dev->s32SensorIndex >= SENSOR_NUM)
	{
		printk("the alias value of %s is invalid. It would be set to 0.\n", np->name);
		return -EINVAL;
	}
	
	g_stSensorDev[sensor_dev->s32SensorIndex] = sensor_dev;

	ret = of_property_read_u32(np, "isp,bind_dev", &sensor_dev->s32IspBindDev);
	if (ret)
	{
		sensor_dev->s32IspBindDev = 0;
	}

	ret = of_property_read_string(np, "control-bus,type", &string);
	if (ret)
	{
		string = sensor_bus_type;
	}

	if (!strncmp(string, "i2c", 5))
	{
		sensor_dev->stCtrlBus.enBusType = SENSOR_CTRL_BUS_I2C;
	}
	else if (!strncmp(string, "ssp", 5))
	{
		sensor_dev->stCtrlBus.enBusType = SENSOR_CTRL_BUS_SSP;
	}
	else
	{
		printk("the \"control-bus,type\" value of %s don't exist.\n", np->name);
		return -EINVAL;
	}

	ret = of_property_read_u32(np, "control-bus,dev", &sensor_dev->stCtrlBus.u32BusNum);
	if (ret)
	{
		sensor_dev->stCtrlBus.u32BusNum = sensor_bus_dev;
	}

	ret = of_property_read_u32(np, "clock-frequency", &node_dev->clk_freq);
	if (ret)
	{
		node_dev->clk_freq = sensor_clk_frequency;	
	}

	if (is_sensor_pinmux_mode(sensor_pinmux_mode)) {
		struct pinctrl *p;
		struct pinctrl_state *state;
		
		p = (struct pinctrl *)devm_pinctrl_get(dev);
        if (IS_ERR(p)) {
            dev_err(dev, "no pinctrl handle\n");
            return -ENOENT;
        }

		state = (struct pinctrl_state *)pinctrl_lookup_state(p, sensor_pinmux_mode);
		if (IS_ERR(state)) {
	     	dev_err(dev, "no %s pinctrl state\n", sensor_pinmux_mode);
	     	devm_pinctrl_put(p);
	     	return -ENOENT;
	    }

		ret = pinctrl_select_state(p, state);
	    if (ret) {
	        dev_err(dev, "failed to activate %s pinctrl state\n", sensor_pinmux_mode);
	        devm_pinctrl_put(p);
	     	return -ENOENT;
	    }
		
		node_dev->p = p;
		node_dev->state = state;
		
	}

	node_dev->clk = of_clk_get(np, 0);
	if (IS_ERR(node_dev->clk))
	{
		dev_err(dev, "sensor cannot get clock\n");
		return -ENOENT;
	}

	node_dev->init_clk_freq = clk_get_rate(node_dev->clk);

	return 0;
}

static int hi35xx_sensor_probe(struct platform_device *pdev)
{
	struct sensor_node_dev *node_dev;
	int index;
	int ret;

    //printk("probe sensor ok.\n");

	node_dev = devm_kzalloc(&pdev->dev, sizeof(*node_dev), GFP_KERNEL);
	if (!node_dev) {
		dev_err(&pdev->dev, "could not allocate sensor node device\n");
			return -ENOMEM;
	}

	node_dev->dev = &pdev->dev;
	platform_set_drvdata(pdev, node_dev);
	
	ret = sensor_probe_of(node_dev);
	if (ret)
	{
		printk("of probe failed!\n");
		return ret;
	}

	sensor_hw_init(pdev);

	index = node_dev->sensor_dev.s32SensorIndex;
	ret = sensor_dev_init(index);
	if (ret)
	{
		printk("sensor init failed!\n");
		return ret;
	}

    return 0;
} 

static int hi35xx_sensor_remove(struct platform_device *pdev)
{
	struct sensor_node_dev *node_dev;
	int index;
	
	node_dev = platform_get_drvdata(pdev);
	index = node_dev->sensor_dev.s32SensorIndex;
	sensor_dev_exit(index);
	sensor_hw_exit(pdev);
	
	//printk("remove sensor ok.\n");
	return 0;
}

static const struct of_device_id hi35xx_sensor_match[] = {
        { .compatible = "hisilicon,hi35xx_sensor" },
        {},
};
MODULE_DEVICE_TABLE(of, hi35xx_sensor_match);

static struct platform_driver hi35xx_sensor_driver = {
        .probe          = hi35xx_sensor_probe,
        .remove         = hi35xx_sensor_remove,
        .driver         = {
                .name   = "hi35xx_sensor",
                .of_match_table = hi35xx_sensor_match,
        },

#ifdef CONFIG_HISI_SNAPSHOT_BOOT
		.suspend		= hi35xx_sensor_suspend,
		.resume 		= hi35xx_sensor_resume,
#endif

};

module_platform_driver(hi35xx_sensor_driver);

MODULE_DESCRIPTION("sensor driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("hisilicon");
