#ifndef __OLED_H
#define __OLED_H
#include "ti_msp_dl_config.h"
#include "OLED_Font.h"
// 声明8x16字体数组
extern const unsigned char OLED_F8x16[][16];

class OLED
{
    public:
    // 初始化有参构造函数(手动传入参数)
    OLED(GPIO_Regs* scl, uint32_t sclpins, GPIO_Regs* sda, uint32_t sdapins)
    {
        _scl = scl;
        _sda = sda;
        _sclpins = sclpins;
        _sdapins = sdapins;
        Init();
    }

    // 初始化无参构造函数（在宏定义存在时使用宏定义的参数）
    OLED()
    {
        #if defined(GPIO_OLED_PORT) && defined(GPIO_OLED_PIN_SCL_PIN) && defined(GPIO_OLED_PIN_SDA_PIN)
            _scl = GPIO_OLED_PORT;
            _sda = GPIO_OLED_PORT;
            _sclpins = GPIO_OLED_PIN_SCL_PIN;
            _sdapins = GPIO_OLED_PIN_SDA_PIN;
            Init();
        #else
            #error "OLED GPIO宏未定义，请定义GPIO_OLED_PORT、GPIO_OLED_PIN_SCL_PIN和GPIO_OLED_PIN_SDA_PIN"
        #endif
    }

    void Init(void); //初始化
    void Clear(void); //清屏
    void ShowChar(uint8_t Line, uint8_t Column,const char Char); //显示字符
    void ShowString(uint8_t Line, uint8_t Column, const char *String); //显示字符串
    void ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length); //显示数字
    void ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length); //显示有符号数字  
    void ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length); //显示十六进制数字
    void ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length); //显示二进制数字

    class I2C
    {
    public:
        void Start(void);   // I2C开始
        void Stop(void);    // I2C停止
        void SendByte(uint8_t Byte); // I2C发送一个字节
    };

    private:

    void WriteCommand(uint8_t Command);
    void WriteData(uint8_t Data);
    void SetCursor(uint8_t Y, uint8_t X);
    uint32_t Pow(uint32_t X, uint32_t Y);

    // SDA 写
    void Send_SDA(bool x)
    {
        (x == 1) ? DL_GPIO_setPins(_sda, _sdapins) : DL_GPIO_clearPins(_sda, _sdapins);
    }

    // SCL 写
    void Send_SCL(bool x)
    {
        (x == 1) ? DL_GPIO_setPins(_scl, _sclpins) : DL_GPIO_clearPins(_scl, _sclpins);
    }

    GPIO_Regs* _scl;
    GPIO_Regs* _sda;
    uint32_t _sclpins;
    uint32_t _sdapins;
    I2C _i2c;
};

#endif

