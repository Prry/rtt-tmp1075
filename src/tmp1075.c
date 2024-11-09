
/*
 * Copyright (c) 2020 panrui <https://github.com/Prry/rtt-tmp1075>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-04-28     panrui      the first version
 */
 
#include <rtthread.h>
#include <rtdevice.h>
#include "tmp1075.h"

#ifdef PKG_USING_TMP1075

#define DBG_TAG "tmp1075"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

/* tmp1075 hardware info */
#define TMP1075_ADDR			0x48 		/* i2c slave address */
#define TMP1075_DIEID			0x7500		/* tmp1075 device id */

/* tmp1075 register */
#define TMP1075_REG_TEMP        0x00
#define TMP1075_REG_CFGR      	0x01
#define TMP1075_REG_LLIM        0x02
#define TMP1075_REG_HLIM       	0x03
#define TMP1075_REG_DIEID       0x0F

/* tmp1075 register value */
#define	TMP1075_POWER_NOR		0xF0
#define	TMP1075_POWER_DOWN		0x01

/* tmp1075 for RT-Thread */
#define	TMP1075_I2C_BUS			"i2c1"		/* i2c linked */
#define TMP1075_DEVICE_NAME		"tmp1075"	/* register device name */

/* tmp1075 for RT-Thread sensor device */
#define	TMP1075_TEMP_RANGE_MAX	(1250)		/* 0.1C */	
#define	TMP1075_TEMP_RANGE_MIN	(-550)		/* 0.1C */
#define TMP1075_TEMP_PEROID     (100)		/* 10ms */

static rt_err_t  tmp1075_read_regs(rt_sensor_t psensor, rt_uint8_t reg, rt_uint8_t *data, rt_uint8_t data_size)
{
    struct rt_i2c_msg msg[2];
	struct rt_i2c_bus_device *i2c_bus = RT_NULL;
	rt_uint32_t slave_addr = 0;
	
	slave_addr = (rt_uint32_t)psensor->config.intf.user_data;	/* get i2c slave address */
	i2c_bus = (struct rt_i2c_bus_device*)psensor->parent.user_data;/* get i2c bus device */ 
	
    msg[0].addr  = (rt_uint8_t)slave_addr;
    msg[0].flags = RT_I2C_WR;
    msg[0].len   = 1;
    msg[0].buf   = &reg;
    msg[1].addr  = (rt_uint8_t)slave_addr;
    msg[1].flags = RT_I2C_RD;
    msg[1].len   = data_size;
    msg[1].buf   = data;

    if(rt_i2c_transfer(i2c_bus, msg, 2) == 2)
	{
        return RT_EOK;
    }
    else
    {
	  	LOG_E("i2c bus read failed!\r\n");
        return -RT_ERROR;
    }
}

static rt_err_t  tmp1075_write_regs(rt_sensor_t psensor, rt_uint8_t reg, rt_uint8_t *data, rt_uint8_t data_size)
{
    struct rt_i2c_msg msg[2];
	struct rt_i2c_bus_device *i2c_bus;
	rt_uint32_t slave_addr = 0;

	slave_addr = (rt_uint32_t)psensor->config.intf.user_data;
	i2c_bus = (struct rt_i2c_bus_device*)psensor->parent.user_data;/* get i2c bus device */   
	
    msg[0].addr		= (rt_uint8_t)slave_addr;
    msg[0].flags	= RT_I2C_WR;
    msg[0].len   	= 1;
    msg[0].buf   	= &reg;
    msg[1].addr  	= (rt_uint8_t)slave_addr;
    msg[1].flags	= RT_I2C_WR | RT_I2C_NO_START;
    msg[1].len   	= data_size;
    msg[1].buf   	= data;
    if(rt_i2c_transfer(i2c_bus, msg, 2) == 2)
	{
        return RT_EOK;
    }
    else
    {
	  	LOG_E("i2c bus write failed!\r\n");
        return -RT_ERROR;
    }
}

static rt_err_t tmp1075_read_temp(rt_sensor_t psensor, float *temp)
{
	rt_uint8_t buf[2] = {0};
	
	if(tmp1075_read_regs(psensor, TMP1075_REG_TEMP, buf, 2) != RT_EOK)
	{
		return -RT_ERROR;
	}
	
	*temp = (float)buf[0];
	if((buf[0]&0x80) != 0)
	{/* negative temperature */
		*temp -= ((buf[1]>>4)&0x0f) * 0.0625f;	/* 0.0625C resolution */
	}
	else
	{/* positive temperature */
		*temp += ((buf[1]>>4)&0x0f) * 0.0625f;
	}
	
	return RT_EOK;
}

static rt_err_t tmp1075_set_power(rt_sensor_t psensor, rt_uint8_t power)
{	
  	rt_err_t ret = RT_EOK;
	rt_uint8_t buf[2] = {0x00};
	
    if (power == RT_SENSOR_POWER_DOWN)
    {
	  	ret = tmp1075_read_regs(psensor, TMP1075_REG_CFGR, buf, 2);/* read register value firstly */
		if (ret != RT_EOK)
		{
			goto __exit;
		}
		buf[0] |= TMP1075_POWER_DOWN;
        ret = tmp1075_write_regs(psensor, TMP1075_REG_CFGR, buf, 2);
    }
    else if (power == RT_SENSOR_POWER_NORMAL)
    {
        ret = tmp1075_read_regs(psensor, TMP1075_REG_CFGR, buf, 2);/* read register value firstly */
		if (ret != RT_EOK)
		{
			goto __exit;
		}
		buf[0] &= TMP1075_POWER_NOR;
        ret = tmp1075_write_regs(psensor, TMP1075_REG_CFGR, buf, 2);
    }
    else
    {
        return -RT_ERROR;
    }
__exit:

    return ret;
}

static RT_SIZE_TYPE tmp1075_polling_get_data(rt_sensor_t psensor, struct rt_sensor_data *sensor_data)
{
  	float temp = 0.0f;
	
	if(psensor->info.type == RT_SENSOR_CLASS_TEMP)
	{/* actual temperature */
	  	if (tmp1075_read_temp(psensor, &temp) == RT_EOK)
		{
		  	sensor_data->type = RT_SENSOR_CLASS_TEMP;
			sensor_data->data.temp = (rt_base_t)(temp*10);
			sensor_data->timestamp = rt_sensor_get_ts();
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
	
    return 1;
}

static RT_SIZE_TYPE tmp1075_fetch_data(struct rt_sensor_device *psensor, void *buf, rt_size_t len)
{
    RT_ASSERT(buf);
	RT_ASSERT(psensor);
	
	//if(psensor->parent.open_flag & RT_DEVICE_FLAG_RDONLY)
	if(psensor->config.mode == RT_SENSOR_MODE_POLLING)
	{
        return tmp1075_polling_get_data(psensor, buf);
    }

    return 0;
}

static rt_err_t tmp1075_control(struct rt_sensor_device *psensor, int cmd, void *args)
{
	rt_err_t	ret = RT_EOK;
	rt_uint8_t	buf[2] = {0x00};
	rt_uint8_t  *p = RT_NULL;
	
    RT_ASSERT(psensor);
	RT_ASSERT(args != RT_NULL);
	
    switch (cmd)
    {
        case RT_SENSOR_CTRL_GET_ID:
	       	ret = tmp1075_read_regs(psensor, TMP1075_REG_DIEID, buf, 2);
			p = (rt_uint8_t*)args;
			*p++ = buf[1];		/* tmp1075 device id is 0x7500 */
			*p = buf[0];
        break;
		
		case RT_SENSOR_CTRL_GET_INFO:
		 	rt_memcpy((rt_uint8_t*)args, (const rt_uint8_t*)&(psensor->info), sizeof(psensor->info));
		break;
		
		case RT_SENSOR_CTRL_SET_POWER:
	  		ret = tmp1075_set_power(psensor, (rt_uint32_t)args & 0xff);
			psensor->config.power = (rt_uint32_t)args & 0xff;
	  	break;
		
        default:
        break;
	}
    return ret;
}

static struct rt_sensor_ops tmp1075_ops =
{
    tmp1075_fetch_data,
    tmp1075_control,
};

int rt_hw_tmp1075_init(const char *name, struct rt_sensor_config *cfg)
{
  	rt_err_t ret = RT_EOK;
	rt_uint8_t buf[2] = {0x00};
	rt_sensor_t sensor_temp = RT_NULL;
	struct rt_i2c_bus_device *tmp1075_i2c_bus = RT_NULL;	/* linked i2c bus */
	
    tmp1075_i2c_bus = rt_i2c_bus_device_find(cfg->intf.dev_name);
    if(tmp1075_i2c_bus == RT_NULL)
    {
        LOG_E("i2c bus device %s not found!\r\n", cfg->intf.dev_name);
		ret = -RT_ERROR;
		goto __exit;
    }	

    /* temperature sensor register */
    {
        sensor_temp = rt_calloc(1, sizeof(struct rt_sensor_device));
        if (sensor_temp == RT_NULL)
		{
			goto __exit;
		}
		rt_memset(sensor_temp, 0x0, sizeof(struct rt_sensor_device));
        sensor_temp->info.type       = RT_SENSOR_CLASS_TEMP;
        sensor_temp->info.vendor     = RT_SENSOR_VENDOR_TI;
        sensor_temp->info.model      = "tmp1075_temp";
        sensor_temp->info.unit       = RT_SENSOR_UNIT_DCELSIUS;
        sensor_temp->info.intf_type  = RT_SENSOR_INTF_I2C;
        sensor_temp->info.range_max  = TMP1075_TEMP_RANGE_MAX;	
        sensor_temp->info.range_min  = TMP1075_TEMP_RANGE_MIN;
        sensor_temp->info.period_min = TMP1075_TEMP_PEROID;	/* read ten times in 1 second */

        rt_memcpy(&sensor_temp->config, cfg, sizeof(struct rt_sensor_config));
        sensor_temp->ops = &tmp1075_ops;
        
        ret = rt_hw_sensor_register(sensor_temp, name, RT_DEVICE_FLAG_RDWR, (void*)tmp1075_i2c_bus/* private data */);
        if (ret != RT_EOK)
        {
            LOG_E("device register err code: %d", ret);
            goto __exit;
        }
    }
   
	/* tmp1075 init */
	ret = tmp1075_read_regs(sensor_temp, TMP1075_REG_DIEID, buf, 2);
	if(ret == RT_EOK)
	{
		rt_uint16_t id = (buf[0]<<8) | buf[1];
		if (id != TMP1075_DIEID)
		{
		  	LOG_E("tmp1075 read device id failed\r\n");
			goto __exit;
		}
	}
	else
	{
		LOG_E("tmp1075 init failed\r\n");
		goto __exit;
	}
    return RT_EOK;

__exit:
	if(sensor_temp)
	{
		rt_free(sensor_temp);
	}
	
    return ret;
}

#endif /* PKG_USING_TMP1075 */
