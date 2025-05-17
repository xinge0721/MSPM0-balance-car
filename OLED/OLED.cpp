#include "OLED.h"   


/*引脚配置*/
 void OLED_W_SCL(uint8_t x)
{
    if(x)
        DL_GPIO_setPins(GPIOB, DL_GPIO_PIN_8);
    else
        DL_GPIO_clearPins(GPIOB, DL_GPIO_PIN_8);
}

 void OLED_W_SDA(uint8_t x)
{
    if(x)
        DL_GPIO_setPins(GPIOB, DL_GPIO_PIN_9);
    else
        DL_GPIO_clearPins(GPIOB, DL_GPIO_PIN_9);
}

/*OLED初始化*/
void OLED_Init(void)
{

}