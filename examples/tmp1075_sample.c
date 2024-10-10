
/*
 * Copyright (c) 2020 panrui <https://github.com/Prry/rtt-tmp1075>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-04-28     panrui      the first version
 */

#include "tmp1075.h"

static void read_temp_entry(void *parameter)
{
    rt_device_t temp_dev = RT_NULL;
    struct rt_sensor_data temp_data;
    rt_size_t res = 0;

	temp_dev = rt_device_find("temp_tmp1075");
    if (temp_dev == RT_NULL)
    {
	  	 rt_kprintf("not found temp_tmp1075 device\r\n");
        return;
    }

    if (rt_device_open(temp_dev, RT_DEVICE_FLAG_RDONLY) != RT_EOK)
    {
        rt_kprintf("open temp_tmp1075 failed\r\n");
        return;
    }
	
    while (1)
    {
		res = rt_device_read(temp_dev, 0, &temp_data, 1);
        if (res == 0)
        {
            rt_kprintf("read data failed! result is %d\n", res);;
			rt_device_close(temp_dev);
            return;
        }
        else
        {
        	rt_kprintf("temp[%d.%dC],timestamp[%d]\r\n", 
					   temp_data.data.temp/10, temp_data.data.temp%10,
					   temp_data.timestamp);
        }

        rt_thread_delay(500);
    }
}

static int temp_read_sample(void)
{
    rt_thread_t temp_thread;

    temp_thread = rt_thread_create("temp_r",
                                     read_temp_entry,
                                     RT_NULL,
                                     1024,
                                     RT_THREAD_PRIORITY_MAX / 2,
                                     20);
    if (temp_thread != RT_NULL)
    {
        rt_thread_startup(temp_thread);
    }

    return RT_EOK;
}
INIT_APP_EXPORT(temp_read_sample);

static int rt_hw_tmp1075_port(void)
{
    struct rt_sensor_config cfg;
    	
	cfg.intf.dev_name = "i2c1"; 		/* i2c bus */
    cfg.intf.user_data = (void *)0x48;	/* i2c slave addr */
    rt_hw_tmp1075_init("tmp1075", &cfg);/* tmp1075 */

    return RT_EOK;
}
INIT_COMPONENT_EXPORT(rt_hw_tmp1075_port);
