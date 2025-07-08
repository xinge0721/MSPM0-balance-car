/**
 * @file APP.h
 * @brief 与上位机通信的应用层协议模块
 * @details
 *  - 本模块提供了一个通用的接口，用于将数据发送到上位机软件（如 FireWater）。
 *  - 它通过一个函数指针来解耦底层的串口发送函数，使得该模块可以轻松移植到不同平台或使用不同的串口。
 *  - 使用流程：
 *    1. 在系统初始化时，调用 APP_Init() 并传入一个 printf 风格的打印函数。
 *    2. 在需要发送数据时，调用 APP_Send()。
 */
#ifndef __APP_H
#define __APP_H
 
#include <stdarg.h> // 引入此头文件以支持可变参数列表，这是 printf 风格函数所必需的。

/**
 * @brief 定义一个函数指针类型 `app_printf_t`
 * @details
 *  - 这个指针用于指向任何一个“printf风格”的函数。
 *  - “printf风格”函数是指第一个参数是格式化字符串，后面跟着可变数量参数的函数。
 *  - 例如，标准库的 `printf` 或我们自己封装的 `u1_printf` 都可以用这个指针类型来表示。
 */
typedef void (*app_printf_t)(const char *format, ...);

/**
 * @brief 初始化本应用模块
 * @details
 *  - 这个函数必须在系统启动后首先被调用，以“注册”一个用于发送数据的底层函数。
 *  - 如果不调用这个函数进行初始化，那么 APP_Send 将无法工作。
 * @param p_printf 一个函数指针，指向一个具体实现的、能发送字符串的函数 (例如: printf, u1_printf)。
 *                 后续 APP_Send 发送的所有数据，最终都将通过这个函数指针所指向的函数来发送出去。
 */
void APP_Init(app_printf_t p_printf);

/**
 * @brief 发送一组浮点数数据到上位机
 * @details
 *  - 此函数会将一个浮点数数组转换成 "数据1,数据2,数据3\n" 这样的格式。
 *  - 这是为了适配 FireWater 这类上位机软件的数据协议。
 *  - 例如，如果传入数组 {1.2f, 3.4f, 5.6f}，它最终会通过注册的打印函数发送字符串 "1.20,3.40,5.60\n"。
 * @param data 指向一个浮点数数组的指针，包含了所有需要发送的数据。
 * @param size 数组中元素的个数。
 */
void APP_Send(float* data, int size);
 
#endif 



