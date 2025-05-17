#ifndef __OLED_H
#define __OLED_H
#include "../ti_msp_dl_config.h"

class OLED
{
    public:

    OLED(GPIO_Regs* scl, GPIO_Regs* sda, uint32_t sclpins, uint32_t sdapins)
    {
        _scl = scl;
        _sda = sda;
        _sclpins = sclpins;
        _sdapins = sdapins;
    }


    // SDA 写
    void Send_SDA(bool x)
    {
        x == 0 ? DL_GPIO_setPins(_sda, _sdapins) : DL_GPIO_clearPins(_sda, _sdapins);
    }

    // SCL 写
    void Send_SCL(bool x)
    {
        x == 0 ? DL_GPIO_setPins(_scl, _sclpins) : DL_GPIO_clearPins(_scl, _sclpins);
    }

    // 开始
    void Start(void)
    private:

    GPIO_Regs* _scl;
    GPIO_Regs* _sda;
    uint32_t _sclpins;
    uint32_t _sdapins;
};

#endif

