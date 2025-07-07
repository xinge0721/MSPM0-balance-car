#ifndef __OLED_H
#define __OLED_H
#include "ti_msp_dl_config.h"


// 全局GPIO配置变量
extern GPIO_Regs* OLED_SCL_PORT;
extern GPIO_Regs* OLED_SDA_PORT;
extern uint32_t _OLED_SCL_PIN;
extern uint32_t _OLED_SDA_PIN;

// 初始化函数 - 手动配置GPIO端口和引脚
void OLED_Init_Custom(GPIO_Regs* scl, uint32_t sclpin, GPIO_Regs* sda, uint32_t sdapin);

// 初始化函数 - 使用宏定义的默认配置
/**
  * @brief  OLED初始化 - 使用宏定义的默认配置
  * @param  无
  * @retval 无
  */
 #if defined(OLED_PORT) && defined(OLED_SCL_PIN) && defined(OLED_SDA_PIN)
	void OLED_Init(void);
 #else
	void OLED_Init(GPIO_Regs* scl, uint32_t sclpin, GPIO_Regs* sda, uint32_t sdapin);
 #endif
 
// 基本OLED控制函数
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, const char Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, const char *String);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);

//OLED SSD1306 I2C 时钟SCL
#define		OLED_SCL_Set()			    (DL_GPIO_setPins(OLED_SCL_PORT, _OLED_SCL_PIN))
#define		OLED_SCL_Clr()				(DL_GPIO_clearPins(OLED_SCL_PORT, _OLED_SCL_PIN))

//----------------------------------------------------------------------------------
//OLED SSD1306 I2C 数据SDA
#define		OLED_SDA_Set()				(DL_GPIO_setPins(OLED_SDA_PORT, _OLED_SDA_PIN))
#define		OLED_SDA_Clr()			    (DL_GPIO_clearPins(OLED_SDA_PORT, _OLED_SDA_PIN))

// 内部I2C控制函数
void OLED_I2C_Start(void);
void OLED_I2C_Stop(void);
void OLED_I2C_SendByte(uint8_t Byte);

// 内部OLED控制函数
void OLED_WriteCommand(uint8_t Command);
void OLED_WriteData(uint8_t Data);
void OLED_SetCursor(uint8_t Y, uint8_t X);
uint32_t OLED_Pow(uint32_t X, uint32_t Y);

#endif

