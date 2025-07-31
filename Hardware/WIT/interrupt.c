#include "interrupt.h"



// 滴答定时器中断服务函��
#if defined UART_BNO08X_INST_IRQHandler
void UART_BNO08X_INST_IRQHandler(void)
{
    uint8_t checkSum = 0;
    extern uint8_t bno08x_dmaBuffer[19];

    DL_DMA_disableChannel(DMA, DMA_BNO08X_CHAN_ID);
    uint8_t rxSize = 18 - DL_DMA_getTransferSize(DMA, DMA_BNO08X_CHAN_ID);

    if(DL_UART_isRXFIFOEmpty(UART_BNO08X_INST) == false)
        bno08x_dmaBuffer[rxSize++] = DL_UART_receiveData(UART_BNO08X_INST);

    for(int i=2; i<=14; i++)
        checkSum += bno08x_dmaBuffer[i];

    if((rxSize == 19) && (bno08x_dmaBuffer[0] == 0xAA) && (bno08x_dmaBuffer[1] == 0xAA) && (checkSum == bno08x_dmaBuffer[18]))
    {
        bno08x_data.index = bno08x_dmaBuffer[2];
        bno08x_data.yaw = (int16_t)((bno08x_dmaBuffer[4]<<8)|bno08x_dmaBuffer[3]) / 100.0;
        bno08x_data.pitch = (int16_t)((bno08x_dmaBuffer[6]<<8)|bno08x_dmaBuffer[5]) / 100.0;
        bno08x_data.roll = (int16_t)((bno08x_dmaBuffer[8]<<8)|bno08x_dmaBuffer[7]) / 100.0;
        bno08x_data.ax = (bno08x_dmaBuffer[10]<<8)|bno08x_dmaBuffer[9];
        bno08x_data.ay = (bno08x_dmaBuffer[12]<<8)|bno08x_dmaBuffer[11];
        bno08x_data.az = (bno08x_dmaBuffer[14]<<8)|bno08x_dmaBuffer[13];
    }
    
    uint8_t dummy[4];
    DL_UART_drainRXFIFO(UART_BNO08X_INST, dummy, 4);

    DL_DMA_setDestAddr(DMA, DMA_BNO08X_CHAN_ID, (uint32_t) &bno08x_dmaBuffer[0]);
    DL_DMA_setTransferSize(DMA, DMA_BNO08X_CHAN_ID, 18);
    DL_DMA_enableChannel(DMA, DMA_BNO08X_CHAN_ID);
}
#endif

#if defined UART_WIT_INST_IRQHandler
void UART_WIT_INST_IRQHandler(void)
{
    uint8_t checkSum, packCnt = 0;
    extern uint8_t wit_dmaBuffer[33];

    DL_DMA_disableChannel(DMA, DMA_WIT_CHAN_ID);
    uint8_t rxSize = 32 - DL_DMA_getTransferSize(DMA, DMA_WIT_CHAN_ID);

    if(DL_UART_isRXFIFOEmpty(UART_WIT_INST) == false)
        wit_dmaBuffer[rxSize++] = DL_UART_receiveData(UART_WIT_INST);

    while(rxSize >= 13)
    {
        for(int i=0; i<rxSize; i++)
        {
            uart0_send_char(wit_dmaBuffer[i]);
            rxflag.rxbuffer[i]=wit_dmaBuffer[i];
        }
        Red_green_xy();
        rxSize -= 11;
        packCnt++;
    }
    
    uint8_t dummy[6];
    DL_UART_drainRXFIFO(UART_WIT_INST, dummy, 6);

    DL_DMA_setDestAddr(DMA, DMA_WIT_CHAN_ID, (uint32_t) &wit_dmaBuffer[0]);
    DL_DMA_setTransferSize(DMA, DMA_WIT_CHAN_ID, 32);
    DL_DMA_enableChannel(DMA, DMA_WIT_CHAN_ID);
}
#endif


double Red_xy[2]={0,0};//xy的pid输出
void Red_green_xy(void)// 坐标处理
{
	uint8_t a = 3;//起使位
	
	if(rxflag.rxbuffer[0]==0x2C)
	{
		if(rxflag.rxbuffer[1]==0x42)
		{	
			if(rxflag.rxbuffer[12]==0x5B)
			{
				//openmv发送是 0x2c,0x42, x x y y 
				uint16_t x =  (rxflag.rxbuffer[a] << 8) | rxflag.rxbuffer[a+1];
				a += 2;//移动去x坐标小数
				uint16_t xx = (rxflag.rxbuffer[a] << 8) | rxflag.rxbuffer[a+1];
				
				a += 3;//移动去y坐标存储的数组
				
				// 组合Y坐标
				uint16_t y = (rxflag.rxbuffer[a] << 8) | rxflag.rxbuffer[a+1];
				a+=2;
				uint16_t yy = (rxflag.rxbuffer[a] << 8) | rxflag.rxbuffer[a+1];
				
				if(rxflag.rxbuffer[2]==0xFE)
				{
					Red_xy[0] = -x-(xx*0.0001);
					
				}
				else {Red_xy[0] = x+(xx*0.0001);}
				
				if(rxflag.rxbuffer[7]==0xFE)
				{
					Red_xy[1] = -y-(yy*0.0001);
				}
				else {Red_xy[1] = y+(yy*0.0001);}
				
				printf("Red:%.4f %.4f",Red_xy[0],Red_xy[1]);
				printf("\r\n");
			}
		}
	}
	//清空rxflag[i].rxbuffer缓冲区数据
	memset(rxflag.rxbuffer,0, sizeof(rxflag.rxbuffer));
}