# 2024-RSOC-DAY2

## 学习内容

今天主要学习了RTThread的内核与线程的基本逻辑和*stateup()*的启动流程

### RTThread的特点：

- 实时性
- 多任务并发性
- 异步事件
- 快速启动

### Linux与RTOS

#### linux 作为通用操作系统

- 对硬件资源消耗大 非实时性 在微控制器的部署上，RTOS的专用性更强一点

#### RTOS的三个版本

| RT-Thread Nano | RT-Thread       | RT-Thread Smart  |
| -------------- | --------------- | ---------------- |
| 适用范围       |                 |                  |
| MCU            | 针对Iot节点设备 | 针对MPU          |
| 包含组件       |
| shell；uart    | 多软件包        | 类似内核的微内核 |

### 嵌入式系统的入门：

#### 裸机开发的过程和弊端

1. 程序永远在一个无限循环中进行，缺少程序之间的调度
2. 中断虽然可以及时响应消息，但终端中无法长时间的处理数据
3. 隐形的延时会影响整个程序的进展，影响整个后台系统

### RTThread内核

1. 具有*并发性 实时性 可维护性 可复用性*
2. 具有 事件驱动 资源共享优点

#### ~~相比于裸机开发~~，RTThread使用线程调度的方式轮转
*假设要做一顿饭，裸机开发是先洗菜，再切菜，然后烧水；*

*RTOS便类似于一边洗菜，一边烧水，洗完菜了再去切菜，根据优先级不同有不同的工作内容*   

### 线程的创建与使用
***静态线程***

```

//静态线程初始化
//第一个函数为线程控制块，那么便定义一个线程控制块
struct rt_thread thread1;
//第二个参数为name ，那么便为线程命名
//"****"
//第三个参数为线程的入口函数，定义入口函数
void thread1_entry(void *parameter){
    //在thread1中写具体要实现的功能
    rt_uint32_t count = 0;
    while(count<10){
        count++;
        rt_pin_write(GPIO_LED_R, LED_HIGH);
        rt_pin_write(GPIO_LED_B, LED_LOW);
        rt_kprintf("thread1_count=%d\n",count);//rt的打印函数
        rt_thread_mdelay(500);//使用rtos的延时函数，该函数可以将线程由就绪改为挂起态
    }

}
void thread2_entry(void *parameter);
//第四个参数是为了给入口函数传递参数当入口函数不需参数时，便可以不传递参数
//第五个参数为线程 栈的起始地址
//可以将其定义为一个数组，该数组为线程设置栈空间、
ALIGN(RT_ALIGN_SIZE)//给对象分配地址空间时，将存放的地址对齐，便于CPU快速访问
char thread_stack[1024] ;//定义的栈空间的大小
//第六个参数为栈空间的大小，可以通过取栈空间的长度获得
//第七个参数为线程的优先级，在RT中优先级为256位，为节省资源可设置成32位
//第八个参数为时间片参数，单位是毫秒：时间片仅在相同优先级下的进程中进行调度
//可以通过线程初始化的返回值判断线程是否初始化成功，线程返回值的类型为rt_err_t
//如果返回值变量不等于RT_EOK，证明初始化成功
static int rtthread_text()
{
    rt_err_t error;

    rt_pin_mode(GPIO_LED_R, PIN_MODE_OUTPUT);
    rt_pin_mode(GPIO_LED_B, PIN_MODE_OUTPUT);

    error=rt_thread_init(&thread1,
                        "thread1",//RT_NAME_MAX 8
                        thread1_entry,
                        RT_NULL,
                        &thread_stack,
                        sizeof(thread_stack),
                        20,
                        10);
    if (error != RT_EOK ) {
        rt_kprintf("thread1 init error\n");
    }
    rt_kprintf("thread1 init success\n");


```
***动态线程***


```
//动态线程创建，动态线程创建不需要设置线程控制块
//动态线程的返回值为rt_thread_t
    rt_thread_t thread2;
//第一个参数为name ，那么便为线程命名
//"****"
//第二个参数为线程的入口函数，定义入口函数
void thread2_entry(void *parameter){
    rt_uint32_t count = 0;
        while(1){//该循环中可以让代码一直运行
            count++;
            rt_pin_write(GPIO_LED_R, LED_LOW);
            rt_pin_write(GPIO_LED_B, LED_HIGH);
            rt_kprintf("thread2_count=%d\n",count);//rt的打印函数
            rt_thread_mdelay(1000);//使用rtos的延时函数，该函数可以将线程由就绪改为挂起态
        }
}
//第三个参数是为了给入口函数传递参数当入口函数不需参数时，便可以不传递参数
//第四个参数为栈空间的大小，可以通过取栈空间的长度获得
//第五个参数为线程的优先级，动态线程的优先级应该大于静态线程优先级
//第六个参数为时间片参数，单位是毫秒：时间片仅在相同优先级下的进程中进行调度
//可以通过线程初始化的返回值判断线程是否初始化成功，线程返回值的类型为rt_thread_t
//如果返回线程句柄等于RT_NULL（空），证明初始化失败

thread2=rt_thread_create("thread2",
                     thread2_entry,
                     RT_NULL,
                     1024,
                     15,
                     10);
    if (thread2 == RT_NULL ) {
        rt_kprintf("thread2 create error\n");
    }
    rt_kprintf("thread2 create success\n");
```


## 学习成果

调用线程，使在打印线程运行状态时，同时存在外设（LED）的调用
*星火一号bsp中有些小问题，LED为低电平🔋，但例程想让亮的时候给的PIN_HIGH，无伤大雅，但是如果一个状态对应一个颜色的话会导致误判断*
