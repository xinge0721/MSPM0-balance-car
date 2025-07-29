#include "gray.h"

uint8_t IN0;
uint8_t IN1, IN2 ,IN3 ,IN4 ,IN5 ,IN6 ,IN7 ,IN8 ,IN9 ,IN10,IN11,IN12;

void gw_gray_serial_read_simple (void)
{
    IN0= DL_GPIO_readPins(Tracking_IN1_PORT,Tracking_IN1_PIN)?1:0;
    IN1= DL_GPIO_readPins(Tracking_IN2_PORT,Tracking_IN2_PIN)?1:0;
    IN2= DL_GPIO_readPins(Tracking_IN3_PORT,Tracking_IN3_PIN)?1:0;
    IN3= DL_GPIO_readPins(Tracking_IN4_PORT,Tracking_IN4_PIN)?1:0;
    IN4= DL_GPIO_readPins(Tracking_IN5_PORT,Tracking_IN5_PIN)?1:0;
    IN5= DL_GPIO_readPins(Tracking_IN6_PORT,Tracking_IN6_PIN)?1:0;
    IN6= DL_GPIO_readPins(Tracking_IN7_PORT,Tracking_IN7_PIN)?1:0;
    IN7= DL_GPIO_readPins(Tracking_IN8_PORT,Tracking_IN8_PIN)?1:0;
    IN8= DL_GPIO_readPins(Tracking_IN9_PORT,Tracking_IN9_PIN)?1:0;
    IN9= DL_GPIO_readPins(Tracking_IN10_PORT,Tracking_IN10_PIN)?1:0;
    IN10= DL_GPIO_readPins(Tracking_IN11_PORT,Tracking_IN11_PIN)?1:0;
    IN11= DL_GPIO_readPins(Tracking_IN12_PORT,Tracking_IN12_PIN)?1:0;
}

int8_t turn_err_k=0;
int turn_err=0;
uint8_t A=0; // 识别到黑线标志位

// 使用示例
int process_sensors()
{
    gw_gray_serial_read_simple();
    
    int result = xunji();
    
    return result;
}

int xunji(void)
{
    if(IN1==1   &&IN2==0&&IN3==0&&IN4==0&&IN5==0&&IN6==0&&IN7==0&&IN8==0&&IN9==0&&IN10==0&&IN11==0&&IN12==0)
    {
        turn_err=55; A=1; 
    }
    else if(   IN1==1&&IN2==1   &&IN3==0&&IN4==0&&IN5==0&&IN6==0&&IN7==0&&IN8==0&&IN9==0&&IN10==0&&IN11==0&&IN12==0)
    {
        turn_err=50;A=1;
    }
    else if(IN1==0&&   IN2==1   &&IN3==0&&IN4==0&&IN5==0&&IN6==0&&IN7==0&&IN8==0&&IN9==0&&IN10==0&&IN11==0&&IN12==0)
    {
        turn_err=45;A=1;
    }
    else if(IN1==0&&   IN2==1&&IN3==1   &&IN4==0&&IN5==0&&IN6==0&&IN7==0&&IN8==0&&IN9==0&&IN10==0&&IN11==0&&IN12==0)
    {
        turn_err=40;A=1;
    }
    else if(IN1==0&&IN2==0&&   IN3==1   &&IN4==0&&IN5==0&&IN6==0&&IN7==0&&IN8==0&&IN9==0&&IN10==0&&IN11==0&&IN12==0)
    {
        turn_err=35;A=1;
    }
    else if(IN1==0&&IN2==0&&   IN3==1&&IN4==1   &&IN5==0&&IN6==0&&IN7==0&&IN8==0&&IN9==0&&IN10==0&&IN11==0&&IN12==0)
    {
        turn_err=30;A=1;
    }
    else if(IN1==0&&IN2==0&&IN3==0&&   IN4==1   &&IN5==0&&IN6==0&&IN7==0&&IN8==0&&IN9==0&&IN10==0&&IN11==0&&IN12==0)
    {
        turn_err=25; A=1;
    }
    else if(IN1==0&&IN2==0&&IN3==0&&   IN4==1&&IN5==1   &&IN6==0&&IN7==0&&IN8==0&&IN9==0&&IN10==0&&IN11==0&&IN12==0)
    {
        turn_err=20; A=1;
    }
    else if(IN1==0&&IN2==0&&IN3==0&&IN4==0&&   IN5==1   &&IN6==0&&IN7==0&&IN8==0&&IN9==0&&IN10==0&&IN11==0&&IN12==0)
    {
        turn_err=15; A=1;
    }
    else if(IN1==0&&IN2==0&&IN3==0&&IN4==0&&   IN5==1&&IN6==1   &&IN7==0&&IN8==0&&IN9==0&&IN10==0&&IN11==0&&IN12==0)
    {
        turn_err=10; A=1;
    }
    else if(IN1==0&&IN2==0&&IN3==0&&IN4==0&&IN5==0&&   IN6==1   &&IN7==0&&IN8==0&&IN9==0&&IN10==0&&IN11==0&&IN12==0)
    {
        turn_err=5; A=1;
    }
    else if(IN1==0&&IN2==0&&IN3==0&&IN4==0&&IN5==0&&   IN6==1&&IN7==1   &&IN8==0&&IN9==0&&IN10==0&&IN11==0&&IN12==0)
    {
        turn_err=0; A=1;
    }
    else if(IN1==0&&IN2==0&&IN3==0&&IN4==0&&IN5==0&&IN6==0&&   IN7==1   &&IN8==0&&IN9==0&&IN10==0&&IN11==0&&IN12==0)
    {
        turn_err=-5; A=1;
    }
    else if(IN1==0&&IN2==0&&IN3==0&&IN4==0&&IN5==0&&IN6==0&&   IN7==1&&IN8==1   &&IN9==0&&IN10==0&&IN11==0&&IN12==0)
    {
        turn_err=-10; A=1;
    }
    else if(IN1==0&&IN2==0&&IN3==0&&IN4==0&&IN5==0&&IN6==0&&IN7==0&&   IN8==1   &&IN9==0&&IN10==0&&IN11==0&&IN12==0)
    {
        turn_err=-15; A=1;
    }
    else if(IN1==0&&IN2==0&&IN3==0&&IN4==0&&IN5==0&&IN6==0&&IN7==0&&   IN8==1&&IN9==1   &&IN10==0&&IN11==0&&IN12==0)
    {
        turn_err=-20; A=1;
    }
    else if(IN1==0&&IN2==0&&IN3==0&&IN4==0&&IN5==0&&IN6==0&&IN7==0&&IN8==0&&   IN9==1   &&IN10==0&&IN11==0&&IN12==0)
    {
        turn_err=-25; A=1;
    }
    else if(IN1==0&&IN2==0&&IN3==0&&IN4==0&&IN5==0&&IN6==0&&IN7==0&&IN8==0&&   IN9==1&&IN10==1   &&IN11==0&&IN12==0)
    {
        turn_err=-30; A=1;
    }
    else if(IN1==0&&IN2==0&&IN3==0&&IN4==0&&IN5==0&&IN6==0&&IN7==0&&IN8==0&&IN9==0&&   IN10==1   &&IN11==0&&IN12==0)
    {
        turn_err=-35; A=1;
    }
    else if(IN1==0&&IN2==0&&IN3==0&&IN4==0&&IN5==0&&IN6==0&&IN7==0&&IN8==0&&IN9==0&&   IN10==1&&IN11==1   &&IN12==0)
    {
        turn_err=-40; A=1;
    }
    else if(IN1==0&&IN2==0&&IN3==0&&IN4==0&&IN5==0&&IN6==0&&IN7==0&&IN8==0&&IN9==0&&IN10==0&&   IN11==1   &&IN12==0)
    {
        turn_err=-45; A=1;
    }
    else if(IN1==0&&IN2==0&&IN3==0&&IN4==0&&IN5==0&&IN6==0&&IN7==0&&IN8==0&&IN9==0&&IN10==0&&   IN11==1&&IN12==1   )
    {
        turn_err=-50; A=1;
    }
    else if(IN1==0&&IN2==0&&IN3==0&&IN4==0&&IN5==0&&IN6==0&&IN7==0&&IN8==0&&IN9==0&&IN10==0&&IN11==0&&   IN12==1   )
    {
        turn_err=-55; A=1;
    }
    else if(IN1==0&&IN2==0&&IN3==0&&IN4==0&&IN5==0&&IN6==0&&IN7==0&&IN8==0&&IN9==0&&IN10==0&&IN11==0&&   IN12==0   )
    {
        A=0; 

    }
	return turn_err;
}	