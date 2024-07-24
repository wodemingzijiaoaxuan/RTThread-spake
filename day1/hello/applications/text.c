/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-07-23     xiaoxiaodao       the first version
 */

#include <board.h>
#include <rtthread.h>
#include "drv_gpio.h"
#ifndef RT_USING_NANO
#include <rtdevice.h>
#endif /* RT_USING_NANO */

#include <board.h>
#include <rtdbg.h>

#define GPIO_LED_B GET_PIN(F, 11)
#define GPIO_LED_R GET_PIN(F, 12)

#define LED_LOW 0x01
#define LED_HIGH 0x00

// 静态线程初始化
// 第一个函数为线程控制块，那么便定义一个线程控制块
struct rt_thread thread1;
// 第二个参数为name ，那么便为线程命名
//"****"
// 第三个参数为线程的入口函数，定义入口函数
void thread1_entry(void *parameter)
{
    // 在thread1中写具体要实现的功能
    rt_uint32_t count = 0;
    while (count < 10)
    {
        count++;
        rt_pin_write(GPIO_LED_R, LED_HIGH);
        rt_pin_write(GPIO_LED_B, LED_LOW);
        rt_kprintf("thread1_count=%d\n", count); // rt的打印函数
        rt_thread_mdelay(500);                   // 使用rtos的延时函数，该函数可以将线程由就绪改为挂起态
    }
}
void thread2_entry(void *parameter);
// 第四个参数是为了给入口函数传递参数当入口函数不需参数时，便可以不传递参数
// 第五个参数为线程 栈的起始地址
// 可以将其定义为一个数组，该数组为线程设置栈空间、
ALIGN(RT_ALIGN_SIZE)     // 给对象分配地址空间时，将存放的地址对齐，便于CPU快速访问
char thread_stack[1024]; // 定义的栈空间的大小
// 第六个参数为栈空间的大小，可以通过取栈空间的长度获得
// 第七个参数为线程的优先级，在RT中优先级为256位，为节省资源可设置成32位
// 第八个参数为时间片参数，单位是毫秒：时间片仅在相同优先级下的进程中进行调度
// 可以通过线程初始化的返回值判断线程是否初始化成功，线程返回值的类型为rt_err_t
// 如果返回值变量不等于RT_EOK，证明初始化成功
static int rtthread_text()
{
    rt_err_t error;

    rt_pin_mode(GPIO_LED_R, PIN_MODE_OUTPUT);
    rt_pin_mode(GPIO_LED_B, PIN_MODE_OUTPUT);

    error = rt_thread_init(&thread1,
                           "thread1", // RT_NAME_MAX 8
                           thread1_entry,
                           RT_NULL,
                           &thread_stack,
                           sizeof(thread_stack),
                           20,
                           10);
    if (error != RT_EOK)
    {
        rt_kprintf("thread1 init error\n");
    }
    rt_kprintf("thread1 init success\n");

    // 动态线程创建，动态线程创建不需要设置线程控制块
    // 动态线程的返回值为rt_thread_t
    rt_thread_t thread2;
    // 第一个参数为name ，那么便为线程命名
    //"****"
    // 第二个参数为线程的入口函数，定义入口函数
    void thread2_entry(void *parameter)
    {
        rt_uint32_t count = 0;
        while (1)
        { // 该循环中可以让代码一直运行
            count++;
            rt_pin_write(GPIO_LED_R, LED_LOW);
            rt_pin_write(GPIO_LED_B, LED_HIGH);
            rt_kprintf("thread2_count=%d\n", count); // rt的打印函数
            rt_thread_mdelay(1000);                  // 使用rtos的延时函数，该函数可以将线程由就绪改为挂起态
        }
    }
    // 第三个参数是为了给入口函数传递参数当入口函数不需参数时，便可以不传递参数
    // 第四个参数为栈空间的大小，可以通过取栈空间的长度获得
    // 第五个参数为线程的优先级，动态线程的优先级应该大于静态线程优先级
    // 第六个参数为时间片参数，单位是毫秒：时间片仅在相同优先级下的进程中进行调度
    // 可以通过线程初始化的返回值判断线程是否初始化成功，线程返回值的类型为rt_thread_t
    // 如果返回线程句柄等于RT_NULL（空），证明初始化失败

    thread2 = rt_thread_create("thread2",
                               thread2_entry,
                               RT_NULL,
                               1024,
                               15,
                               10);
    if (thread2 == RT_NULL)
    {
        rt_kprintf("thread2 create error\n");
    }
    rt_kprintf("thread2 create success\n");

    /*现在的线程都仅仅处于初始状态，
               想要将线程挂载到链表中启动则需要使用rt_thread_startup()*/
    // 在RTThread中，rt_thread_t是一个结构体指针变量*rt_thread_t，
    // 所以startup里需要传入静态线程的指针变量
    rt_thread_startup(&thread1);
    // 在RTThread中，rt_thread_t是一个结构体指针变量*rt_thread_t，
    // 对于动态线程，只需要将句柄写入
    rt_thread_startup(thread2);
    // 如此两个线程的状态均已改为就绪态，之后的具体实现功能便可以在入口函数中编写
    return RT_EOK;
}

MSH_CMD_EXPORT(rtthread_text, rtthread_text);

