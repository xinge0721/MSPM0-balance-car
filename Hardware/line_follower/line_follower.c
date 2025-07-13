#include "line_follower.h"

// 读取循迹板引脚电平
uint16_t line_follower_read(void)
{
    uint16_t line_value = 0;

    if (IN1)  line_value |= (1 << 0);
    if (IN2)  line_value |= (1 << 1);
    if (IN3)  line_value |= (1 << 2);
    if (IN4)  line_value |= (1 << 3);
    if (IN5)  line_value |= (1 << 4);
    if (IN6)  line_value |= (1 << 5);
    if (IN7)  line_value |= (1 << 6);
    if (IN8)  line_value |= (1 << 7);
    if (IN9)  line_value |= (1 << 8);
    if (IN10) line_value |= (1 << 9);
    if (IN11) line_value |= (1 << 10);
    if (IN12) line_value |= (1 << 11);

    return line_value;
}

int8_t A;

// 返回偏差角度
int16_t line_follower_get_error(void)
{
    uint16_t line_value = line_follower_read();
    int16_t error = 0;

    A = 1; // 默认为1

    switch(line_value)
    {
        // 0代表低电平，1代表高电平
        case 0b000000000000: A = 0; error = 0; break;      // 都不亮
        case 0b000000000001: error = 55; break;    // IN1
        case 0b000000000011: error = 50; break;    // IN1 | IN2
        case 0b000000000010: error = 45; break;    // IN2
        case 0b000000000110: error = 40; break;    // IN2 | IN3
        case 0b000000000100: error = 35; break;    // IN3
        case 0b000000001100: error = 30; break;   // IN3 | IN4
        case 0b000000001000: error = 25; break;    // IN4
        case 0b000000011000: error = 20; break;   // IN4 | IN5
        case 0b000000010000: error = 15; break;   // IN5
        case 0b000000110000: error = 10; break;   // IN5 | IN6
        case 0b000000100000: error = 5; break;    // IN6
        case 0b000001100000: error = 0; break;    // IN6 | IN7
        case 0b000001000000: error = -5; break;   // IN7
        case 0b000011000000: error = -10; break; // IN7 | IN8
        case 0b000010000000: error = -15; break; // IN8
        case 0b000110000000: error = -20; break; // IN8 | IN9
        case 0b000100000000: error = -25; break; // IN9
        case 0b001100000000: error = -30; break; // IN9 | IN10
        case 0b001000000000: error = -35; break; // IN10
        case 0b011000000000: error = -40; break;// IN10 | IN11
        case 0b010000000000: error = -45; break;// IN11
        case 0b110000000000: error = -50; break;// IN11 | IN12
        case 0b100000000000: error = -55; break;// IN12
        default: break; // 其他情况，error为0，A为1
    }

    return error;
}






