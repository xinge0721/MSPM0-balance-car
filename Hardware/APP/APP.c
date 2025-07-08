/**
 * @file APP.c
 * @brief 与上位机通信的应用层协议模块的实现
 * @details 详细实现了 APP.h 中声明的函数。
 */
#include "APP.h"
#include <stdio.h> // 引入标准输入输出库，主要为了使用 snprintf 函数

// 定义一个静态的（仅本文件可见）函数指针变量。
// 这个指针将用于存储由 APP_Init 函数传递进来的、实际的串口打印函数地址。
// 初始化为 NULL 是一个好习惯，可以防止在未初始化时被意外调用。
static app_printf_t app_serial_printf = NULL;

/**
 * @brief 初始化函数实现
 * @param p_printf 外部传入的、要用于发送数据的打印函数
 */
void APP_Init(app_printf_t p_printf)
{
	// 将外部传入的函数指针 p_printf 赋值给我们文件内部的静态指针 app_serial_printf。
	// 这样，我们就“记住”了应该用哪个函数来发送数据。
	app_serial_printf = p_printf;
}

/**
 * @brief 发送函数实现
 * @param data 需要发送的浮点数数组
 * @param size 数组元素的个数
 */
void APP_Send(float* data, int size)
{
	// --- 安全性检查 ---
	// 1. 检查 app_serial_printf 是否已经被初始化（即是否已经调用过 APP_Init）。
	// 2. 检查传入的数组指针是否为空。
	// 3. 检查数组大小是否为正数。
	// 如果以上任何一个条件不满足，就直接返回，避免程序崩溃。
	if (app_serial_printf == NULL || data == NULL || size <= 0)
	{
		return; 
	}

	// 定义一个字符数组作为缓冲区，用于临时存放我们要拼接的字符串。
	// 比如，我们要发送 {1.1, 2.2, 3.3}，我们会先在这里拼出 "1.10,2.20,3.30\n" 这个字符串。
	// 256字节对于大多数应用来说足够了。
	char buffer[256]; 
	// `offset` 是一个游标，记录当前我们已经写入到缓冲区的哪个位置了。
	int offset = 0;   

	// 遍历传入的浮点数数组中的每一个数字
	for (int i = 0; i < size; i++)
	{
		// 检查缓冲区剩余空间是否还够用。
		// -20 是一个保守估计，确保有足够空间存放一个浮点数（如 "-12345.78"）、一个逗号和一个换行符。
		// 如果空间不足，就停止拼接，避免缓冲区溢出。
		if (offset >= sizeof(buffer) - 20) break; 

		// 使用 snprintf 将一个浮点数格式化为字符串，并拼接到 buffer 的末尾。
		// `buffer + offset` 表示从缓冲区的 `offset` 位置开始写入。
		// `sizeof(buffer) - offset` 告诉 snprintf 最多能写多少字节，防止溢出。
		// "%.2f" 表示格式化为带两位小数的浮点数。
		// `written` 会返回实际写入的字符数。
		int written = snprintf(buffer + offset, sizeof(buffer) - offset, "%.2f", data[i]);
		if (written > 0) // 如果成功写入
		{
			// 更新游标位置，移动到新写入字符串的末尾。
			offset += written;
		}

		// 如果当前处理的不是数组的最后一个数字，我们就在后面加一个逗号。
		if (i < size - 1)
		{
			// 再次检查是否还有空间放逗号
			if (offset < sizeof(buffer) - 1)
			{
				buffer[offset++] = ','; // 放入逗号，并让游标后移一位。
			}
		}
	}

	// --- 循环结束后，拼接工作完成，现在准备发送 ---
	
	// 检查是否还有空间放一个换行符 `\n` 和一个字符串结束符 `\0`。
	if (offset < sizeof(buffer) - 2)
	{
		buffer[offset++] = '\n'; // 在字符串末尾添加换行符
		buffer[offset] = '\0'; // 添加C语言字符串必需的结束符
		
		// 调用我们之前保存的打印函数，将整个拼接好的字符串发送出去。
		app_serial_printf("%s", buffer);
	}
}


// =================================================================================
// |                                 测试代码示例                                    |
// =================================================================================

// 定义几个静态全局变量，仅用于测试函数
static float a = 5, b = 10, c = 20;

/**
 * @brief FireWater上位机通信协议的测试函数示例
 * @details
 *  - 这个函数演示了如何直接使用注册的打印函数来发送符合特定格式的数据。
 *  - 每次调用，它都会修改 a, b, c 的值，然后以 "a,b,c\n" 的格式发送出去。
 *  - 这是一种不使用 APP_Send，而是直接调用 `app_serial_printf` 的用法示例。
 */
void FireWater_Test(void)
{
	// 更新测试数据
	a += 100;
	b += 50;
	c += 10;

	// 在调用前，同样需要检查打印函数是否已经被初始化
	if (app_serial_printf != NULL)
	{
		// 直接调用打印函数，发送格式化后的字符串
		app_serial_printf("%.2f,%.2f,%.2f\n", a, b, c);
	}
} 




