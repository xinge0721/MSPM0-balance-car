#include "HCSR04.h"
#include "ti_msp_dl_config.h"
#include "system/delay/delay.h"
#include <stdint.h>


// volatile关键字告诉编译器，msHcCount的值随时可能在当前代码上下文之外被改变（例如：在中断服务程序中）
// 所以每次使用它的时候，都应该从内存中重新读取，而不是使用可能存在于寄存器中的旧值。
volatile uint32_t msHcCount = 0; // 用于在定时器中断中累计毫秒数

float distance = 0; // 存储计算出的距离值，单位是厘米
volatile uint8_t SR04_Flag = 0; // 一个标志位，为1时表示一次成功的超声波测距已经完成

// 超声波模块的状态
volatile SR04_State sr04_state = SR04_IDLE;
// 超声波模块的初始时间
volatile uint32_t HCSR04_StartTime = 0;


/******************************************************************
 * 函 数 名 称：Get_TIMER_Count
 * 函 数 说 明：获取从Open_Timer调用开始到当前的总时间（防中断竞争安全版本）
 * 函 数 形 参：无
 * 函 数 返 回：总计时时间，单位是微秒(us)
 * 作       者：LCKFB
 * 备       注：通过两次读取msHcCount并检查其一致性，来避免在读取过程中
 *             发生timer溢出中断而导致的计时错误。
******************************************************************/
uint32_t Get_TIMER_Count(void)
{
    uint32_t ms1, ms2, us;
    do {
        ms1 = msHcCount;  // 第一次读取毫秒数
        us = DL_TimerG_getTimerCount(TIMER_0_INST); // 读取微秒数
        ms2 = msHcCount;  // 第二次读取毫秒数
    } while (ms1 != ms2); // 如果两次读取的毫秒数不同，说明在读取us期间发生了中断，需重新读取

    return ms1 * 1000 + us;
}


/******************************************************************
 * 函 数 名 称：SR04_GetLength
 * 函 数 说 明：触发一次超声波测量并获取距离
 * 函 数 形 参：无
 * 函 数 返 回：测量到的距离（单位：厘米）。如果超时则返回0。
 * 作       者：LCKFB
 * 备       注：此函数为单次测量版本。
******************************************************************/
float SR04_GetLength(void)
{
    // 设置一个基于毫秒的超时计数器，防止在ECHO引脚没有响应时程序卡死
    // HC-SR04的最大探测距离约为4-5米，回波时间最多需要 5m*2/340m/s ≈ 30ms。
    // 无回波时，ECHO会保持高电平约38ms。所以60ms的超时是比较安全的。
    uint32_t TimeOut_ms = 60;

    // lc_printf("SR04: Start measurement...\r\n");
    msHcCount = 0; // 每次测量前，都把毫秒计数器清零
    SR04_Flag = 0; // 将完成标志清零，准备开始新的测量
    distance = 0.0f;
    sr04_state = SR04_IDLE; // 每次测量开始前，都将状态机复位到空闲状态

    // --- 关键步骤：在触发前，先配置好中断，确保状态干净 ---
    DL_GPIO_clearInterruptStatus(SR04_PORT, SR04_Echo_PIN); // 清除可能残留的中断标志
    DL_GPIO_enableInterrupt(SR04_PORT, SR04_Echo_PIN);      // 使能ECHO引脚的GPIO中断

    // --- 开始触发超声波模块 ---
    // 根据SR04模块的数据手册，需要给TRIG引脚一个至少10us的高电平脉冲
    SR04_TRIG(0); // 1. 先将TRIG引脚拉低
    delay_1us(2); // 2. 延时一小段时间（2us足够）
    SR04_TRIG(1); // 3. 将TRIG引脚拉高，开始发送脉冲
    delay_1us(15); // 4. 保持高电平超过10us
    SR04_TRIG(0); // 5. 拉低TRIG引脚，完成触发
    // --- 触发结束 ---
    // lc_printf("SR04: Trigger pulse sent.\r\n");

    // 等待测量完成。测量完成的标志是 SR04_Flag 被中断服务程序设置为1
    // 同时检查msHcCount实现的超时
    while (SR04_Flag == 0 && msHcCount < TimeOut_ms)
    {
        // 等待中断或超时
    }


    if (SR04_Flag == 0) // 如果循环是因为超时而退出的
    {
        DL_GPIO_disableInterrupt(SR04_PORT, SR04_Echo_PIN); // 超时后也应关闭中断
        sr04_state = SR04_IDLE; // 超时后也要将状态机复位
        // lc_printf("SR04: Timeout! (msHcCount=%d)\r\n", msHcCount);
        // 这意味着在指定时间内没有收到ECHO的下降沿，测量失败
        return 0.0f; // 返回0.0表示超时或无效距离
    }

    // 如果程序能走到这里，说明 SR04_Flag == 1，测量成功
    // distance 变量已经在中断服务程序中被计算和赋值了
    // lc_printf("SR04: Success!\r\n");
    return distance; // 返回计算出的距离值
}
