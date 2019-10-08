#include "uart.h"
#include "stc8.h"
#include "intrins.h"
#include "config.h"
#include <string.h>

/*
     串口1 -- GSM   P36 P37
	 串口2 -- WIFI  P10 P11
*/

/* 判断是否是2的倍数 */
#define IsNotPowerOF2(x)			((x)&(x-1))
//u8 usart1_date,usart2_date;

// NB： Timer 1 -> 串口1	   115200    GSM
void Uart1Isr() interrupt 4 using 1
{
	if (TI)
	{
		TI = 0;
#if enableUart1SendInterrupt
		if (COM1.B_TX_busy == TRUE && COM1.TX_read != COM1.TX_write)
		{
			SBUF = TX1_Buffer[COM1.TX_read];
			++COM1.TX_read;
		}
		else {
			COM1.B_TX_busy = FALSE;
		}
#else
		COM1.B_TX_busy = FALSE;
#endif 
	}

	if (RI)
	{
		RI = 0;

		do {
			/* 判断是否已经接受完毕 <==> lock */
			if (COM1.RX_Cnt
				&& COM1.RX_TimeOut <= TimeOutSet2)
			{
				break;
			}

			/* 正常接收 */
			if (COM1.RX_Cnt < COM_RX1_Lenth - 1)
			{
				RX1_Buffer[COM1.RX_Cnt++] = SBUF;
				//usart1_date = SBUF;
			}
			COM1.RX_TimeOut = TimeOutSet1;
		} while (0);
	}
}

void p_uart1_init(void)
{
	AIR_TX = 1;
	AIR_RX = 1;
	PORT_MODE_SET(AIR_RX_G, AIR_RX_P, PORT_MODE_AD);

	/* 选择串口一的功能引脚为 RxD:P3.6  TxD:P3.7 */
  //  P_SW1 = 0x40;
	//disableUartInterrupts();
	SCON = 0x50;		                // 8位数据，可变波特率
	AUXR |= 0x40;		                // 定时器1时钟1为FOSC,即1T
	AUXR &= 0xFE;		                // 串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		                // 设定定时器1工作模式0 为16位自动重装方式
	TL1 = (65536 - (FOSC / 4 / BAUD));     // 设置定时初值 这里设置12为晶振
	TH1 = (65536 - (FOSC / 4 / BAUD)) >> 8;   // 设置定时初值
	TR1 = 1;		                    // 启动定时器1

	enableUartInterrupts();
}

void p_uart1_send_byte(char dat)
{
#if enableUart1SendInterrupt
	if (COM1.TX_read != COM1.TX_write + 1)
	{
		TX1_Buffer[COM1.TX_write] = dat;	//装发送缓冲
		++COM1.TX_write;
	}

	if (COM1.B_TX_busy == FALSE)		//空闲
	{
		COM1.B_TX_busy = TRUE;		    //标志忙
		SBUF = dat;
		++COM1.TX_read;
	}
#else
	while (COM1.B_TX_busy);
	COM1.B_TX_busy = TRUE;
	SBUF = dat;
#endif
}

/**
  * ***********************************************************************
  * @brief	通过串口1发送数据
  *
  *	@param  p: 需要发送的数据
  *	@param  length: 发送的数据长度
  *
  * @retval void:
  *
  * @attention	: none
  * ***********************************************************************
  */
void p_uart1_send_data(char *p, uint16_t length)
{
	uint8_t i = 0;

	for (i = 0; i < length; i++)
	{
		p_uart1_send_byte(p[i]);
	}
}

void p_uart1_send_string(char *p)
{
	while (*p)
	{
		p_uart1_send_byte(*p);
		++p;
	}
}

//BLE：Timer 2 -> 串口2   115200    WIFI
void Uart2Isr() interrupt 8 using 1
{
	/* 接收 */
	if (S2CON & 0x01)
	{
		S2CON &= ~0x01;

		RX2_Buffer[COM2.RX_Cnt++] = S2BUF;
		//usart2_date = S2BUF;
		COM2.RX_TimeOut = TimeOutSet1;
	}

	/* 发送 */
	if (S2CON & 0x02)
	{
		S2CON &= ~0x02;
#if enableUart2SendInterrupt
		if (COM2.B_TX_busy == TRUE && COM2.TX_read != COM2.TX_write)
		{
			S2BUF = TX2_Buffer[COM2.TX_read & 0x7F];
			++COM2.TX_read;
		}
		else {
			COM2.B_TX_busy = FALSE;
		}
#else
		COM2.B_TX_busy = FALSE;
#endif
	}
}


/**
  * ***********************************************************************
  * @brief	串口2初始化，115200
  *
  *	@param  :
  *
  * @retval void:
  *
  * @confirm	:
  * @attention	: none
  * ***********************************************************************
  */

void p_uart2_init(void)
{
	WIFI_RX = 1;
	WIFI_TX = 1;
	PORT_MODE_SET(WIFI_RX_G, WIFI_RX_P, PORT_MODE_AD);

	//disableUar2tInterrupts();
	S2CON = 0x50;		                // 8位数据，可变波特率
	AUXR |= 0x04;		                // 定时器2时钟1为FOSC,即1T 
	T2L = (65536 - (FOSC / 4 / BAUD));     // 设置定时初值 这里设置12为晶振
	T2H = (65536 - (FOSC / 4 / BAUD)) >> 8;   // 设置定时初值
	AUXR |= 0x10;		                // 启动定时器2	  

	enableUart2Interrupts();
}


void p_uart2_send_byte(char dat)
{
#if enableUart2SendInterrupt
	if (COM2.TX_read != COM2.TX_write + 1)
	{
		TX2_Buffer[COM2.TX_write & 0x7F] = dat;	//装发送缓冲
		++COM2.TX_write;
	}

	if (COM2.B_TX_busy == FALSE)		//空闲
	{
		COM2.B_TX_busy = TRUE;		    //标志忙
		S2BUF = dat;
		++COM2.TX_read;
	}

#else
	while (COM2.B_TX_busy);
	COM2.B_TX_busy = 1;
	S2BUF = dat;
#endif
}

static uint16_t uart2_send_idx = 0;
static uint16_t uart1_send_idx = 0;
void p_uart2_send_data(char *p, uint8_t length)
{
	
	for (uart2_send_idx = 0; uart2_send_idx < length; uart2_send_idx++)
	{
		p_uart2_send_byte(p[uart2_send_idx]);
	}
}

/* 用于WIFI特殊数据格式发送到GSM中的 */
void p_uart1_send_data_backslash(char* p, uint16_t length)
{
	for (uart1_send_idx = 0; uart1_send_idx < length; ++uart1_send_idx)
	{
		/* 过滤\r\n */
		if ('\r' == p[uart1_send_idx]
			|| '\n' == p[uart1_send_idx])
		{
			continue;
		}

		if ('\"' == p[uart1_send_idx])
		{
			p_uart1_send_byte('\\');
		}
		p_uart1_send_byte(p[uart1_send_idx]);
	}
}

void p_uart2_send_string(char *p)
{
	while (*p)
	{
		p_uart2_send_byte(*p);
		++p;
	}
}



#if 0
void p_uart1_uninit(void)
{
	/* 选择串口一的功能引脚为 RxD:P1.6  TxD:P1.7 */
	// SCON = 0x00;
	//TMOD = 0x00;		
	//TL1 = 0x00;		 
	//TH1 = 0x00;		 
	// AUXR = 0x00;		
	TF1 = 0;
	TR1 = 0;

	/*PORT_MODE_SET(LR_TX_G, LR_TX_P, PORT_MODE_PP);
	PORT_MODE_SET(LR_RX_G, LR_RX_P, PORT_MODE_PP);
	LR_TX = 0;
	LR_RX = 0;*/

	//disableUartInterrupts();
}


void p_uart2_uninit(void)
{
	/*PORT_MODE_SET(AIR_TX_G, AIR_TX_P, PORT_MODE_PP);
	PORT_MODE_SET(AIR_RX_G, AIR_RX_P, PORT_MODE_PP);
	AIR_TX = 0;
	AIR_RX = 0;*/
}



void p_uart1_init(void)
{
	LR_TX = 1;
	LR_RX = 1;
	PORT_MODE_SET(LR_RX_G, LR_RX_P, PORT_MODE_AD);

	/* 选择串口一的功能引脚为 RxD:P3.6  TxD:P3.7 */
	// P_SW1 = 0x40;
	disableUartInterrupts();
	/* 9600 */
	//SCON = 0x50;		// 8位数据,可变波特率
	//AUXR |= 0x40;		// 定时器1时钟为Fosc,即1T
	//AUXR &= 0xFE;		// 串口1选择定时器1为波特率发生器
	//TMOD &= 0x0F;		// 设定定时器1为16位自动重装方式
	//TL1 = 0xC7;			// 设定定时初值
	//TH1 = 0xFE;			// 设定定时初值
	//ET1 = 0;			// 禁止定时器1中断
	//TR1 = 1;			// 启动定时器1
	/* 115200 */
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设定定时器1为16位自动重装方式
	TL1 = 0xC7;			// 设定定时初值
	TH1 = 0xFE;			// 设定定时初值
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1

	enableUartInterrupts();
}

void p_uart2_init(void)
{
	AIR_RX = 1;
	AIR_TX = 1;
	PORT_MODE_SET(AIR_RX_G, AIR_RX_P, PORT_MODE_AD);

	disableUartInterrupts();

	S2CON = 0x50;		// 8位数据,可变波特率
	AUXR |= 0x04;		// 定时器2时钟为Fosc,即1T
	T2L = 0xE6;		    // 设定定时初值
	T2H = 0xFF;		    // 设定定时初值
	AUXR |= 0x10;		// 启动定时器2

	IE2 |= 0x01;
	enableUartInterrupts();
}


#endif