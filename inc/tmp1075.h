
/*
 * Copyright (c) 2020 panrui <https://github.com/Prry/rtt-tmp1075>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-04-28     panrui      the first version
 */

#ifndef _TMP1075_H_
#define _TMP1075_H_

#include <rtthread.h>
#include <rtdevice.h>

extern int rt_hw_tmp1075_init(const char *name, struct rt_sensor_config *cfg);

#endif /* _TMP1075_H_ */
