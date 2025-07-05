#include "HTS221.h"
#include "../Serial/Serial.h"
	
// 发送数据函数
#define HTS221_send_date(date, size) uart0_SendArray(date, size)

static uint8_t HTS221_date[10] = {0};


// 初始化HTS221
void HTS221_Init(void)
{
	HTS221_date[0] = 0x55;
	HTS221_date[1] = 0x55;
	for(int i = 2; i < 10; i++) {
		HTS221_date[i] = 0x00;
	}
}

void HTS221_turn(uint16_t angle, uint16_t speed, uint8_t ID)
{
	// 确保数据在有效范围内
	if (angle > 1000 || speed > 30000)
	{
			return;
	}

	HTS221_date[2] = ID;
	// Length
	HTS221_date[3] = 0x07;
	// Command
	HTS221_date[4] = 0x01;
	// Data
	// 参数 1：角度的低八位。
	HTS221_date[5] = angle & 0xFF;

	// 参数 2：角度的高八位。范围 0~1000，对应舵机角度的 0~240°，即舵机可变化
	// 的最小角度为 0.24度。
	HTS221_date[6] = angle >> 8;

	// 参数 3：时间低八位。
	HTS221_date[7] = speed & 0xFF;

	// 参数 4：时间高八位，时间的范围 0~30000毫秒。该命令发送给舵机，舵机将
	// 在参数时间内从当前角度匀速转动到参数角度。该指令到达舵机后，舵机会立
	// 即转动。
	HTS221_date[8] = speed >> 8;

	// CRC
	// Checksum = ~ (ID + Length + Cmd+ Prm1 + ... Prm N)若括号内的计算和超出 255
	// 则取后 8 位，即对 255 取反。
	HTS221_date[9] = ~ (ID + HTS221_date[3] + HTS221_date[4] + HTS221_date[5] + 
							HTS221_date[6] + HTS221_date[7] + HTS221_date[8]);

	HTS221_send_date(HTS221_date, 10);
}

// 停止舵机
void HTS221_stop(uint8_t ID)
{
	HTS221_date[2] = ID;

	// Length
	HTS221_date[3] = 0x03;
	// Command
	HTS221_date[4] = 0x0C;

	HTS221_date[5] = 0x00;
	HTS221_date[6] = 0x00;
	HTS221_date[7] = 0x00;
	HTS221_date[8] = 0x00;
	// CRC
	// Checksum = ~ (ID + Length + Cmd+ Prm1 + ... Prm N)若括号内的计算和超出 255
	// 则取后 8 位，即对 255 取反。
	HTS221_date[9] = ~ (ID + HTS221_date[3] + HTS221_date[4] + HTS221_date[5] + 
							HTS221_date[6] + HTS221_date[7] + HTS221_date[8]);

	HTS221_send_date(HTS221_date, 10);
}


// 获取舵机角度请求
// 指令名 SERVO_POS_READ指令值 28数据长度 3
void HTS221_getAngle(uint8_t ID)
{
	HTS221_date[2] = ID;

	// Length
	HTS221_date[3] = 0x03;
	// Command
	HTS221_date[4] = 0x1C;

	HTS221_date[5] = 0x00;
	HTS221_date[6] = 0x00;
	HTS221_date[7] = 0x00;
	HTS221_date[8] = 0x00;
	// CRC
	// Checksum = ~ (ID + Length + Cmd+ Prm1 + ... Prm N)若括号内的计算和超出 255
	// 则取后 8 位，即对 255 取反。
	HTS221_date[9] = ~ (ID + HTS221_date[3] + HTS221_date[4] + HTS221_date[5] + 
						HTS221_date[6] + HTS221_date[7] + HTS221_date[8]);

	HTS221_send_date(HTS221_date, 10);	//发送请求
}

// 设置舵机角度限制
// 指令名 SERVO_ANGLE_LIMIT_WRITE指令值 20数据长度 7
void HTS221_setAngleLimit(uint16_t minAngle, uint16_t maxAngle, uint8_t ID)
{
	// 确保最小角度小于最大角度
	if (minAngle >= maxAngle)
	{
		return;
	}
	
	// 确保角度在有效范围内
	if (minAngle > 1000 || maxAngle > 1000)
	{
		return;
	}
	
	HTS221_date[2] = ID;
	
	// Length
	HTS221_date[3] = 0x07;
	// Command
	HTS221_date[4] = 0x14;
	
	// 最小角度低八位
	HTS221_date[5] = minAngle & 0xFF;
	// 最小角度高八位
	HTS221_date[6] = (minAngle >> 8) & 0xFF;
	// 最大角度低八位
	HTS221_date[7] = maxAngle & 0xFF;
	// 最大角度高八位
	HTS221_date[8] = (maxAngle >> 8) & 0xFF;
	
	// CRC
	// Checksum = ~ (ID + Length + Cmd+ Prm1 + ... Prm N)若括号内的计算和超出 255
	// 则取后 8 位，即对 255 取反。
	HTS221_date[9] = ~ (ID + HTS221_date[3] + HTS221_date[4] + HTS221_date[5] + 
						HTS221_date[6] + HTS221_date[7] + HTS221_date[8]);
	
	HTS221_send_date(HTS221_date, 10);
}