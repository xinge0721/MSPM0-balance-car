#include "Tracking.h"
#include "ti_msp_dl_config.h"

// uint16_t Tracking_get_pin_value(void)
// {
//     uint16_t pin_value = 0;
//     // DL_GPIO_readPins 返回的是一个包含引脚状态的32位掩码, 而不是简单的 0 或 1
//     // 我们需要检查它是否非零来判断引脚电平
//     if (IN1) pin_value |= (1 << 11);
//     if (IN2) pin_value |= (1 << 10);
//     if (IN3) pin_value |= (1 << 9);
//     if (IN4) pin_value |= (1 << 8);
//     if (IN5) pin_value |= (1 << 7);
//     if (IN6) pin_value |= (1 << 6);
//     if (IN7) pin_value |= (1 << 5);
//     if (IN8) pin_value |= (1 << 4);
//     if (IN9) pin_value |= (1 << 3);
//     if (IN10) pin_value |= (1 << 2);
//     if (IN11) pin_value |= (1 << 1);
//     if (IN12) pin_value |= (1 << 0);
//     return pin_value;
// }


// TrackingStatus xunji(void)
// {
//     // 使用静态变量来保存上一次的偏差值，以便在丢失线时保持状态
//     static int last_deviation = 0;
//     TrackingStatus status;

//     uint16_t pin_value = Tracking_get_pin_value();

//     // 默认认为检测到了线，除非所有传感器都读数为0
//     status.line_detected = 1;

//     // 根据12个循迹传感器的状态值计算偏差'error'
//     // 传感器从左到右为 IN1 -> IN12
//     // 二进制位'1'表示传感器检测到黑线
//     switch (pin_value)
//     {
//         // 左偏
//         case 0b100000000000: last_deviation = 55;  break; // IN1
//         case 0b110000000000: last_deviation = 50;  break; // IN1, IN2
//         case 0b010000000000: last_deviation = 45;  break; // IN2
//         case 0b011000000000: last_deviation = 40;  break; // IN2, IN3
//         case 0b001000000000: last_deviation = 35;  break; // IN3
//         case 0b001100000000: last_deviation = 30;  break; // IN3, IN4
//         case 0b000100000000: last_deviation = 25;  break; // IN4
//         case 0b000110000000: last_deviation = 20;  break; // IN4, IN5
//         case 0b000010000000: last_deviation = 15;  break; // IN5
//         case 0b000011000000: last_deviation = 10;  break; // IN5, IN6
//         case 0b000001000000: last_deviation = 5;   break; // IN6

//         // 居中
//         case 0b000001100000: last_deviation = 0;   break; // IN6, IN7

//         // 右偏
//         case 0b000000100000: last_deviation = -5;  break; // IN7
//         case 0b000000110000: last_deviation = -10; break; // IN7, IN8
//         case 0b000000010000: last_deviation = -15; break; // IN8
//         case 0b000000011000: last_deviation = -20; break; // IN8, IN9
//         case 0b000000001000: last_deviation = -25; break; // IN9
//         case 0b000000001100: last_deviation = -30; break; // IN9, IN10
//         case 0b000000000100: last_deviation = -35; break; // IN10
//         case 0b000000000110: last_deviation = -40; break; // IN10, IN11
//         case 0b000000000010: last_deviation = -45; break; // IN11
//         case 0b000000000011: last_deviation = -50; break; // IN11, IN12
//         case 0b000000000001: last_deviation = -55; break; // IN12

//         // 所有传感器都未检测到线
//         case 0b000000000000:status.line_detected = 0;break;
//     }

//     status.deviation = last_deviation;
//     return status;
// }	
