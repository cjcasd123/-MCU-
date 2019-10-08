#include "stc8.h"
#include "config.h"
#include "event.h"
#include "uart.h"
#include <intrins.h>
#include <string.h>
#include "watermark.h"
#include "mainproc.h"

//extern u8 usart1_date,usart2_date;
/*
      最原始版本
   1 只是对于MCU的功能测试（不包含其他的设备工能）
   2 存在一些测试的日志
*/

void main(void)
{
	EVNT_Handle evnt = 0;
	u8    last_rxd_stat = 0x00;
	u8	  last_rxd_change_cnt = 0x00;
	/* 正式执行程序 */
	p_appc_all_init();

	/* 输出编译日期 */
	//p_uart1_send_string(__DATE__" "__TIME__" Compiled.");

	p_delay_10ms(100);

	while (1)
	{
		/* 防止后面的事件没有被询问到 */
		evnt = evnt >= EVNT_MAX ? 0 : evnt + 1;

		/* 事件轮询 */
		for (; evnt < EVNT_MAX; ++evnt)
		{
			if (p_envt_get_event(evnt))
			{
				p_envt_clear_event(evnt);
				break;
			}
		}

		/* 事件处理 */
		switch (evnt)
		{
		case EVNT_INIT:
			break;
		case EVNT_WATERMARK:
			p_watermark_ctrl(g_config.m_enbale_sy1, g_config.m_enbale_sy2);
			if ((g_config.m_enbale_sy1 == 0) && (g_config.m_enbale_sy2 == 0))   // 防止保留上次还有的数据
			{
				g_sy1_shark_cnt = 0;
				g_sy2_shark_cnt = 0;
			}
			evnt = 0;
			break;
		case EVNT_SLEEP:
			/* 睡眠任务 */
			p_uart1_send_string("OK2");
			p_delay_10ms(100);
			p_appc_task_sleep_proc(); // 睡醒后进行发送一次数据
			p_uart1_send_string("OK3");
			evnt = 0;                 // 这个一定要清0
			break;

		default:
			break;
		}

		/* 串口1接收处理与Air相连接 */
		if (COM1.RX_TimeOut
			&& COM1.RX_TimeOut <= TimeOutSet2)
		{
			/* 防止在没有接收到数据的时候，重复进入处理函数 */
			COM1.RX_TimeOut = 0;
			/* 处理串口数据（解析来自GSM的数据） */
			p_proc_data_parser_from_air();
			memset(RX1_Buffer, 0, COM1.RX_Cnt);
			COM1.RX_Cnt = 0;
		}

		/* 串口2接收处理与wifi相连接 */
		if (COM2.RX_TimeOut
			&& COM2.RX_TimeOut <= TimeOutSet2)
		{
			/* 防止在没有接收到数据的时候，重复进入处理函数 */
			COM2.RX_TimeOut = 0;
			/* 处理串口数据(打包WIFI数据和MCU自身数据) */
			p_proc_data_parser_from_loRa();
			memset(RX2_Buffer, 0, COM2.RX_Cnt + 1);
			COM2.RX_Cnt = 0;
			p_uart2_send_string("OK2");
		}
#if 0
		/* 定时发送数据包到GSM */
		if (g_heart_sign == heart_send)
		{
			p_proc_data_parser_from_loRa();
			memset(RX2_Buffer, 0, COM2.RX_Cnt + 1);
			COM2.RX_Cnt = 0;
			g_heart_sign = heart_clear;
		}
#endif
#if 0
		/* 再进行建立一个无串口2返回数值时接收MCU的数据（加一个协议进去或者加个定时器） */
		if (usart1_date == 1)
		{
			/* 处理串口数据 */
			p_proc_data_parser_from_loRa();
			//p_uart1_send_string("OK1");
			usart1_date = 0;
		}

		/* 建立一个烧写固件的转发 */
		// 判断是否要进入下载模式（这个就是转换到WIFI的固件）目前是直接IO口数据转换就行（串1RX--串2RX）   
		//但是满足的条件的含义未知？？
		if (IAP_RXD == last_rxd_stat)
		{
			last_rxd_stat = IAP_RXD;
			if (last_rxd_change_cnt++ > 10)
			{
				//EA = 0;
				disableInterrupts();
				WIFI_RST = 0;
				WIFI_SET = 0;
				WIFI_PU = 0;
				Delay1000ms();
				WIFI_PU = 1;
				WIFI_RST = 1;

				while (1)
				{
					WIFI_RXD = IAP_RXD;
					IAP_TXD = WIFI_TXD;
				}
			}
		}
#endif

	}
}


/*

void Delay1000ms()		//@12.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 61;
	j = 225;
	k = 62;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

*/

