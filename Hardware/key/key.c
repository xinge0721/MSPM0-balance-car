#include "key.h"

/**************************************************************************************************** 
*                             长按、单击、双击定义
* 长按事件：任何大于 KEY_LONG_PRESS_TIME 
* 单击事件：按下时间不超过 KEY_LONG_PRESS_TIME 且 释放后 KEY_WAIT_DOUBLE_TIME 内无再次按下的操作
* 双击事件：俩次短按时间间隔小于KEY_WAIT_DOUBLE_TIME，俩次短按操作合并为一次双击事件。
* 特殊说明：
*          1.短按和长按时间间隔小于 KEY_WAIT_DOUBLE_TIME，响应一次单击和长按事件，不响应双击事件
*          2.连续2n次短按，且时间间隔小于 KEY_WAIT_DOUBLE_TIME，响应为n次双击
*          3.连续2n+1次短按，且时间间隔小于 KEY_WAIT_DOUBLE_TIME，且最后一次KEY_WAIT_DOUBLE_TIME内无操作，
*				响应为n次双击 和 一次单击事件
****************************************************************************************************/
#define KEY_LONG_PRESS_TIME    50   // 20ms * 50 = 1s
#define KEY_WAIT_DOUBLE_TIME   25   // 20ms * 25 = 500ms

#define KEY_PRESSED_LEVEL      0  // 按键按下是电平为低
/**************************************************************************************************** 
*                             局部函数定义
****************************************************************************************************/
static KEY_PinLevel_TypeDef KEY_ReadPin(uint8_t data);   // 按键读取按键的电平函数
static void KEY_GetAction_PressOrRelease(uint8_t data); // 获取按键是按下还是释放，保存到结构体

/**************************************************************************************************** 
*                             全局变量
****************************************************************************************************/
// 按键配置数组，支持5个按键（KEY1-KEY5）
KEY_Configure_TypeDef KeyCfg[] = {
    {0, KEY_Action_Release, KEY_Status_Idle, KEY_Event_Null, KEY_ReadPin}, // KEY1
    {0, KEY_Action_Release, KEY_Status_Idle, KEY_Event_Null, KEY_ReadPin}, // KEY2
    {0, KEY_Action_Release, KEY_Status_Idle, KEY_Event_Null, KEY_ReadPin}, // KEY3
    {0, KEY_Action_Release, KEY_Status_Idle, KEY_Event_Null, KEY_ReadPin}, // KEY4
    {0, KEY_Action_Release, KEY_Status_Idle, KEY_Event_Null, KEY_ReadPin}, // KEY5
};

/**************************************************************************************************** 
*                             函数定义
****************************************************************************************************/
// 按键读取按键的电平函数，适配MSPM0平台
static KEY_PinLevel_TypeDef KEY_ReadPin(uint8_t data) {
    if (data > 4) return KEY_PinLevel_High; // 返回无效值，支持5个按键（0-4）
    
    switch(data) {
        case 0: 
            return (KEY_PinLevel_TypeDef)(KEY_key1() ? KEY_PinLevel_High : KEY_PinLevel_Low);
        case 1: 
            return (KEY_PinLevel_TypeDef)(KEY_key2() ? KEY_PinLevel_High : KEY_PinLevel_Low);
        case 2: 
            return (KEY_PinLevel_TypeDef)(KEY_key3() ? KEY_PinLevel_High : KEY_PinLevel_Low);
        case 3: 
            return (KEY_PinLevel_TypeDef)(KEY_key4() ? KEY_PinLevel_High : KEY_PinLevel_Low);
        case 4: 
            return (KEY_PinLevel_TypeDef)(KEY_key5() ? KEY_PinLevel_High : KEY_PinLevel_Low);
        default: 
            return KEY_PinLevel_High;
    }
}


// 获取按键是按下还是释放，保存到结构体
static void KEY_GetAction_PressOrRelease(uint8_t data) // 根据实际按下按钮的电平去把它换算成虚拟的结果
{
	if(KeyCfg[data].KEY_ReadPin_Fcn(data) == KEY_PRESSED_LEVEL)
	{
		KeyCfg[data].KEY_Action = KEY_Action_Press;
	}
	else
	{
		KeyCfg[data].KEY_Action =  KEY_Action_Release;
	}
}

//按键初始化函数 - MSPM0平台
void KEY_Init(void) 
{ 
    // MSPM0平台的GPIO初始化已在系统配置文件中完成
    // 此处保持空函数，确保接口兼容性
    // 如需自定义初始化，可在此处添加相关代码
}

/**************************************************************************************************** 
*                             读取按键状态机
****************************************************************************************************/
void KEY_ReadStateMachine(uint8_t data)
{
	KEY_GetAction_PressOrRelease(data);
	
	switch(KeyCfg[data].KEY_Status)
	{
		//状态：没有按键按下
		case KEY_Status_Idle:
			if(KeyCfg[data].KEY_Action == KEY_Action_Press)
			{
				KeyCfg[data].KEY_Status = KEY_Status_Debounce;
				KeyCfg[data].KEY_Event = KEY_Event_Null;
			}
			else
			{
				KeyCfg[data].KEY_Status = KEY_Status_Idle;
				KeyCfg[data].KEY_Event = KEY_Event_Null;
			}
			break;
			
		//状态：消抖
		case KEY_Status_Debounce:
			if(KeyCfg[data].KEY_Action == KEY_Action_Press)
			{
				KeyCfg[data].KEY_Status = KEY_Status_ConfirmPress;
				KeyCfg[data].KEY_Event = KEY_Event_Null;
			}
			else
			{
				KeyCfg[data].KEY_Status = KEY_Status_Idle;
				KeyCfg[data].KEY_Event = KEY_Event_Null;
			}
			break;	


		//状态：继续按下
		case KEY_Status_ConfirmPress:
			if( (KeyCfg[data].KEY_Action == KEY_Action_Press) && ( KeyCfg[data].KEY_Count >= KEY_LONG_PRESS_TIME))
			{
				KeyCfg[data].KEY_Status = KEY_Status_ConfirmPressLong;
				KeyCfg[data].KEY_Event = KEY_Event_Null;
				KeyCfg[data].KEY_Count = 0;
			}
			else if( (KeyCfg[data].KEY_Action == KEY_Action_Press) && (KeyCfg[data].KEY_Count < KEY_LONG_PRESS_TIME))
			{
				KeyCfg[data].KEY_Count++;
				KeyCfg[data].KEY_Status = KEY_Status_ConfirmPress;
				KeyCfg[data].KEY_Event = KEY_Event_Null;
			}
			else
			{
				KeyCfg[data].KEY_Count = 0;
				KeyCfg[data].KEY_Status = KEY_Status_WaiteAgain;// 按短了后释放
				KeyCfg[data].KEY_Event = KEY_Event_Null;

			}
			break;	
			
		//状态：一直长按着
		case KEY_Status_ConfirmPressLong:
			if(KeyCfg[data].KEY_Action == KEY_Action_Press) 
			{   // 一直等待其放开
				KeyCfg[data].KEY_Status = KEY_Status_Idle;
				KeyCfg[data].KEY_Event = KEY_Event_LongPress;
				KeyCfg[data].KEY_Count = 0;
			}
			else
			{				
				KeyCfg[data].KEY_Status = KEY_Status_ConfirmPressLong;
				KeyCfg[data].KEY_Event = KEY_Event_Null;
				KeyCfg[data].KEY_Count = 0;

			}
			break;	
			
		//状态：等待是否再次按下
		case KEY_Status_WaiteAgain:
			if((KeyCfg[data].KEY_Action != KEY_Action_Press) && ( KeyCfg[data].KEY_Count >= KEY_WAIT_DOUBLE_TIME))
			{   // 第一次短按,且释放时间大于KEY_WAIT_DOUBLE_TIME
				KeyCfg[data].KEY_Count = 0;
				KeyCfg[data].KEY_Status = KEY_Status_Idle;  
				KeyCfg[data].KEY_Event = KEY_Event_SingleClick;// 响应单击
				
			}
			else if((KeyCfg[data].KEY_Action != KEY_Action_Press) && ( KeyCfg[data].KEY_Count < KEY_WAIT_DOUBLE_TIME))
			{// 第一次短按,且释放时间还没到KEY_WAIT_DOUBLE_TIME
				KeyCfg[data].KEY_Count ++;
				KeyCfg[data].KEY_Status = KEY_Status_WaiteAgain;// 继续等待
				KeyCfg[data].KEY_Event = KEY_Event_Null;
				
			}
			else // 第一次短按,且还没到KEY_WAIT_DOUBLE_TIME 第二次被按下
			{
				KeyCfg[data].KEY_Count = 0;
				KeyCfg[data].KEY_Status = KEY_Status_SecondPress;// 第二次按下
				KeyCfg[data].KEY_Event = KEY_Event_Null;
			}
			break;		
		case KEY_Status_SecondPress:
			if( (KeyCfg[data].KEY_Action == KEY_Action_Press) && ( KeyCfg[data].KEY_Count >= KEY_LONG_PRESS_TIME))
			{
				KeyCfg[data].KEY_Status = KEY_Status_ConfirmPressLong;// 第二次按的时间大于 KEY_LONG_PRESS_TIME
				KeyCfg[data].KEY_Event = KEY_Event_SingleClick; // 先响应单击
				KeyCfg[data].KEY_Count = 0;
			}
			else if( (KeyCfg[data].KEY_Action == KEY_Action_Press) && ( KeyCfg[data].KEY_Count < KEY_LONG_PRESS_TIME))
			{
				KeyCfg[data].KEY_Count ++;
				KeyCfg[data].KEY_Status = KEY_Status_SecondPress;
				KeyCfg[data].KEY_Event = KEY_Event_Null;
			}
			else 
			{// 第二次按下后在 KEY_LONG_PRESS_TIME内释放
					KeyCfg[data].KEY_Count = 0;
					KeyCfg[data].KEY_Status = KEY_Status_Idle;
					KeyCfg[data].KEY_Event = KEY_Event_DoubleClick; // 响应双击
			}
			break;	
		default:
			break;
	}

}
