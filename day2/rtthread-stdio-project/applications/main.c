/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-5-10      ShiHao       first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#define DBG_TAG "main"
#define DBG_LVL         DBG_LOG
#include <rtdbg.h>

/* 配置 LED 灯引脚 */
#define GPIO_LED_B              GET_PIN(F, 11)      // PF11 :  LED_B        --> LED
#define GPIO_LED_R              GET_PIN(F, 12)      // PF12 :  LED_R        --> LED
/* */
int main(void)
{
    unsigned int count = 1;

    /* 设置 LED 引脚为输出模式 */
    rt_pin_mode(GPIO_LED_R, PIN_MODE_OUTPUT);
    rt_pin_mode(GPIO_LED_B, PIN_MODE_OUTPUT);

    while (1)
    {
        rt_pin_write(GPIO_LED_R, PIN_HIGH);
        rt_pin_write(GPIO_LED_B, PIN_LOW);
        rt_thread_mdelay(1000);
        rt_pin_write(GPIO_LED_R, PIN_LOW);
        rt_pin_write(GPIO_LED_B, PIN_HIGH);
        rt_thread_mdelay(1000);

    }

    return 0;
}

