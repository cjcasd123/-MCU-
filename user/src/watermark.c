#include "intrins.h"
#include "config.h"
#include "uart.h"
#include "watermark.h"



// S1(P3.2): INT0 下降沿触发
void INT0_Isr() interrupt 0 using 1
{
	if (g_config.m_enbale_sy1 == TRUE)
	{
		g_sy1_shark_cnt++;
	}
}

// S2(P3.3): INT1 下降沿触发
void INT1_Isr() interrupt 2 using 1
{
	if (g_config.m_enbale_sy2 == TRUE)
	{
		g_sy2_shark_cnt++;
	}
}

void p_watermark_init(void)
{
	disableInterrupts();
	/* INT0 */
	IT0 = 1;            // 使能INT0下降沿中断
	EX0 = 1;            // 使能INT0中断

	/* INT1 */
	IT1 = 1;
	EX1 = 1;

	enableInterrupts();
}


void p_watermark_ctrl(uint8_t sy1, uint8_t sy2)
{
	1 == sy1 ? (EX0 = 1) : (EX0 = 0);
	1 == sy2 ? (EX1 = 1) : (EX1 = 0);
}



