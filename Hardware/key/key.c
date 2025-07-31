#include "key.h"

void relay_off(void)
{
    DL_GPIO_setPins(OLED_PORT,Relay_PIN_0_PIN);
}

void relay_on(void)
{
    DL_GPIO_clearPins(OLED_PORT,Relay_PIN_0_PIN);
}