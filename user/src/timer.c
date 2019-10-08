#include "stc8.h"
#include "intrins.h"
#include "config.h"
#include "event.h"
#include "wdg.h"

u32		g_time_ms = 0x00;
u8 g_heart_beat, g_heart_sign;

static uint8_t s_evnt_cnt = 0;



void p_delay_10ms(uint32_t ms)
{
	ms += my_time();
	while (ms > my_time())
	{

	}
}


void p_sysclk_init(void)
{
	/*选择内部24MHz高精度IRC*/
	P_SW2 = 0x80;
	CKSEL = 0x00;                               //选择内部IRC ( 默认 )
	CLKDIV = 0x02;								// 2分频 24MHz/2 = 12Mhz
	P_SW2 = 0x00;
}


/**
  * ***********************************************************************
  * @brief	定时器初始化
  *
  *	@param  :
  *
  * @retval void:
  *
  * @attention	: none
  * ***********************************************************************
  */
void p_timer_init(void)
{
	disableInterrupts();

	/* Timer0 初始化 时间戳10ms */
	AUXR &= 0x7F;		// 定时器时钟12T模式
	TMOD &= 0xF0;		// 设置定时器模式
	TL0 = 0xF0;			// 设置定时初值
	TH0 = 0xD8;			// 设置定时初值
	TF0 = 0;			// 清除TF0标志
	TR0 = 1;			// 定时器0开始计时

	/* 使能定时器中断 */
	ET0 = 1;	                   // 使能定时器0中断
	enableInterrupts();
}

#if 0
void p_timer_uninit(void)
{
	/* Timer0 初始化 时间戳1ms */
	TR0 = 0;		               // 定时器0开始计时

	/* 使能定时器中断 */
	ET0 = 0;	                   // 使能定时器0中断
}
#endif

/* 定时器0 时间戳 */
void TM0_Isr() interrupt 1 using 3
{
	/* 10ms */
	++g_time_ms;
	++s_evnt_cnt;
	s_evnt_cnt %= 100;

	/* 定时心跳包 */
	++g_heart_beat;
	if (g_heart_beat == heart_time)
	{
		g_heart_sign = heart_send;
		g_heart_beat = 0;
	}
		

	/* 喂狗 */
	//WDG_RELOAD();

	/* 串口超时监测 */
	if (COM2.RX_TimeOut > TimeOutSet2)
	{
		--COM2.RX_TimeOut;
	}

	if (COM1.RX_TimeOut > TimeOutSet2)
	{
		--COM1.RX_TimeOut;

		/* 为了防止黏包，在WiFi在发送数据的时候不进行数据上报 */
		s_evnt_cnt = 0;
	}

	
}
