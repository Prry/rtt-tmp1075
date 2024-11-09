
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

#if defined(RT_VERSION_CHECK)
    #if (RTTHREAD_VERSION >= RT_VERSION_CHECK(5, 0, 2))
        #define RT_SIZE_TYPE   rt_ssize_t
    #else
        #define RT_SIZE_TYPE   rt_size_t
    #endif
#endif

extern int rt_hw_tmp1075_init(const char *name, struct rt_sensor_config *cfg);

#endif /* _TMP1075_H_ */
