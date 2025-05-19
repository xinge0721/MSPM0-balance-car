//////////////////////////////////////////////////////////////////////////////////	 
//  文 件 名   : oled.c
//  版 本 号   : v2.0
//  作    者   : Torris
//  生成日期   : 2024-07-08
//  最近修改   : 
//  功能描述   : 0.96寸OLED 接口演示例程(MSPM0G系列)
//  驱动IC     : SSD1306/SSD1315
//              说明: 
//              ----------------------------------------------------------------
//              GND    电源地
//              VCC    接3.3v电源
//              SCL    PA12（时钟）
//              SDA    PA13（数据）    
//******************************************************************************/
#include "ti_msp_dl_config.h"
#include "./Hardware/OLED/OLED.h"
OLED oled(GPIO_OLED_PORT,
        GPIO_OLED_PIN_SCL_PIN,
        GPIO_OLED_PORT,
        GPIO_OLED_PIN_SDA_PIN);
OLED oled2;
int main( void )
{
    SYSCFG_DL_init();
   
   while(1)
   {
    oled.ShowString(1,1,"Hello World");
    oled2.ShowString(2,1,"Hello World");
    oled.ShowString(3,1,"Hello World");
    oled2.ShowString(4,1,"Hello World");
   }
}
