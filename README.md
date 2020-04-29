# TMP1075 温度传感器驱动软件包



## 1 简介

tmp1075 软件包是基于RT-Thread sensor框架实现的一个驱动包。基于该软件包，RT-Thread应用程序可以使用标准的sensor接口访问tmp1075，获取传感器数据。



### 1.1 目录结构

| 名称       | 说明                  |
| ---------- | --------------------- |
| docs       | 文档目录              |
| examples   | 例子目录              |
| inc        | 头文件目录            |
| src        | 源代码目录            |
| LICENSE    | 许可证文件            |
| SConscript | RT-Thread默认构建脚本 |



### 1.2 许可证

tmp1075 软件包遵循 Apache license v2.0 许可，详见 `LICENSE` 文件。

<br>

## 2 传感器介绍

tmp1075 是 Texas Instruments（德州仪器）公司开发的一款i2c接口的数字式温度传感器，具有供电范围广、功耗低、精度高优点。tmp1075与经典lm75、tmp75等xx75系列的温度传感器硬件上pin对pin完全兼容，软上基本兼容，稍作修改代码即可兼容使用。



| 功能 | 量程     | 分辨率  | 精度 |
| ---- | -------- | ------- | ---- |
| 温度 | -55—127℃ | 0.0625℃ | ±1℃  |

<br>

## 3 支持情况



| 包含设备     | 温度计 |
| ------------ | ------ |
| **通信接口** |        |
| IIC          | √      |
| SPI          |        |
| **工作模式** |        |
| 轮询         | √      |
| 中断         |        |
| FIFO         |        |
| **电源模式** |        |
| 掉电         | √      |
| 低功耗       |        |
| 普通         | √      |



<br>

## 4 使用说明

### 4.1 依赖

- RT-Thread 4.0.0+

- sensor 框架组件

- I2C 驱动，tmp1075设备使用 I2C 进行数据通讯，需要系统 I2C 驱动框架支持

  

### 4.2 获取软件包

使用 tmp1075 package 需要在 RT-Thread 的包管理器中选择它，具体路径如下。然后让 RT-Thread 的包管理器自动更新，或者使用 `pkgs --update` 命令更新包到 BSP 中。如需使用示例程序，则使能<code>Enable tmp1075 sample</code>。

```
RT-Thread online packages --->
    peripheral libraries and drivers --->
        sensors drivers --->
            [*] TMP1075 digital temperature sensor for i2c interface.
            		[*] Enable tmp1075 sample
                    Version (latest)  --->
```

>  **Version**：软件包版本选择，默认选择最新版本。 



### 4.3 初始化

tmp1075软件包初始化函数如下所示：

```
int rt_hw_tmp1075_init(const char *name, struct rt_sensor_config *cfg);
```

该函数需要由用户调用，函数主要完成的功能有:

- 根据配置信息配置i2c名称、i2c地址等（可增加其他配置信息），然后初始化设备

- 注册相应的传感器设备，完成 tmp1075设备的注册

  

**参考示例：**

```
#include "tmp1075.h"

static int rt_hw_tmp1075_port(void)
{
    struct rt_sensor_config cfg;
    	
	cfg.intf.dev_name = "i2c1"; 		/* i2c bus */
    cfg.intf.user_data = (void *)0x48;	/* i2c slave addr */
    rt_hw_tmp1075_init("tmp1075", &cfg);/* tmp1075 */

    return RT_EOK;
}
INIT_COMPONENT_EXPORT(rt_hw_tmp1075_port);
```



### 4.4 读取数据

tmp1075软件包基于sensor框架，sensor框架继承于RT-Thread标准设备框架，可以使用RT-Thread标准设备接口"open/read"读取传感器数据。



**参考伪代码:**

```
temp_dev = rt_device_find("temp_tmp1075");
rt_device_open(temp_dev, RT_DEVICE_FLAG_RDONLY)；
rt_device_read(temp_dev, 0, &sensor_data, 1);
```



### 4.5 msh/finsh测试

#### 查看设备注册

```
msh >list_device
device           type         ref count
-------- -------------------- ----------
temp_tmp Sensor Device        1            
i2c1     I2C Bus              0       
pin      Miscellaneous Device 0       
uart4    Character Device     2       
```

>注：
>
>完整设备名称为“temp_tmp1075”，终端显示有长度限制



#### 运行例程周期打印数据

```
 \ | /
- RT -     Thread Operating System
 / | \     4.0.1 build Apr 28 2020
 2006 - 2019 Copyright by rt-thread team
[I/sensor] rt_sensor init success
temp[27.1C],timestamp[2]
msh >temp[27.1C],timestamp[502]
temp[27.1C],timestamp[1002]
temp[27.0C],timestamp[1502]
```



#### 使用RTT自带的测试命令

* 探测设备

```
msh >sensor probe temp_tmp1075
[I/sensor.cmd] device id: 0x7500!
```

> 注：
>
> tmp1075设备id是16bit，“sensor”函数中，存放设备id变量为8bit，需改更改为16bit，否则输出id有误。sensor框架默认设备id为8bit？

* 获取传感器信息

```
msh >sensor info
vendor :11
model  :tmp1075_temp
unit   :8
range_max :1250
range_min :-550
period_min:100
fifo_max  :0
```

> 注：
>
> tmp1075器件厂商为德州仪器，sensor框架暂未提供，现暂定序号为11，已在RT-Thread源码上PR。

* 读取温度数据

```
msh >sensor read 3
[I/sensor.cmd] num:  0, temp: 27.8C, timestamp: 3679
[I/sensor.cmd] num:  1, temp: 27.9C, timestamp: 3690
[I/sensor.cmd] num:  2, temp: 28.0C, timestamp: 3700
```

<br>

## 5 注意事项

暂无

<br>

## 6 联系方式

- 维护：[Acuity](https://github.com/Prry)
- 主页：<https://github.com/Prry/rtt-tmp1075>    