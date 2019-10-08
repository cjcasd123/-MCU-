#include "config.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "intrins.h"
#include "mainproc.h"



u8 g_sy_shark_sign;

idata COMx_Define	                COM1 = { 0x00 };
idata COMx_Define	                COM2 = { 0x00 };

xdata u8					TX1_Buffer[COM_TX1_Lenth] = { 0x00 };	//发送缓冲
xdata u8 	                RX1_Buffer[COM_RX1_Lenth] = { 0x00 };	//接收缓冲

xdata u8					TX2_Buffer[COM_TX2_Lenth] = { 0x00 };	//发送缓冲
xdata u8 	                RX2_Buffer[COM_RX2_Lenth] = { 0x00 };	//接收缓冲

xdata u8					g_pre_recv_pkg[COM_RX2_Lenth] = { 0 };	// 上一次接收的数据包

st_now_config               g_config = { 0x00 };
//idata char*					g_p_boot_reason = NULL;

xdata st_pkg_to_luat		g_pkg_to_luat = { 0 };
idata st_item_from_luat*	g_p_item_from_luat = NULL;

u8			                g_sy1_shark_cnt = 0x00;
u8			                g_sy2_shark_cnt = 0x00;

/*************************        函数声明        *************************/

/* 正初始化 */
void p_appc_all_init(void)
{
	/* 12MHz */
	p_sysclk_init();

	/* 管脚初始化(外部中断IO口) */
	PORT_MODE_SET(SY1_G, SY1_P, PORT_MODE_HZ);
	PORT_MODE_SET(SY2_G, SY2_P, PORT_MODE_HZ);

	/* GSM的电源IO口 */
	PORT_MODE_SET(AIR_POWER_CTRL_G, AIR_POWER_CTRL_P, PORT_MODE_PP);
	PORT_MODE_SET(AIR_POWERKEY_G, AIR_POWERKEY_P, PORT_MODE_PP);

	/*  WIFI的电源IO口 */
	PORT_MODE_SET(WIFI_EN_G, WIFI_EN_P, PORT_MODE_PP);
	PORT_MODE_SET(WIFI_RST_G, WIFI_RST_P, PORT_MODE_PP);
	PORT_MODE_SET(WIFI_UPDATE_G, WIFI_UPDATE_P, PORT_MODE_PP);

	/* 串口初始化 */
	p_uart1_init();
	p_uart2_init();	

	/* 定时器初始化 */
	p_timer_init();
	g_time_ms = 0;

	/* 外部中断配置 */
	p_watermark_init();

	/* 初始化看门狗 */
	//WDG_RELOAD();
	//p_watchdog_init();

	/* 开启所有中断和外部中断 */
	enableInterrupts();
	
	/* 打包固定的格式 */
	p_pkg_data_init();

	/* GSM电源引脚 */
	AIR_POWER_CTRL = 0;
	AIR_POWERKEY = 0;   // 做一个引脚的释放
	//p_delay_10ms(120);
	//AIR_POWERKEY = 1;

	/* WIFI电源引脚 */
	WIFI_EN = 1;
	WIFI_RST = 1;
	WIFI_UPDATE = 1;

}


/* 反初始化 */
void p_appc_all_deinit()
{
	
	/* 各种中断的关闭 */
	disableUartInterrupts();
	disableInterrupts();
	disableExtiInterrupt();
	disableUar2tInterrupts();
	

	/* 管脚初始化 */
	PORT_MODE_SET(0, 0, PORT_MODE_PP); P00 = 0;
	PORT_MODE_SET(0, 1, PORT_MODE_PP); P01 = 0;
	PORT_MODE_SET(0, 2, PORT_MODE_PP); P02 = 0;
	PORT_MODE_SET(0, 3, PORT_MODE_PP); P03 = 0;
	PORT_MODE_SET(1, 0, PORT_MODE_PP); P10 = 0; // 串口2
	PORT_MODE_SET(1, 1, PORT_MODE_PP); P11 = 0;	// 串口2
	PORT_MODE_SET(1, 2, PORT_MODE_PP); P12 = 1; // WIFI_RST
	PORT_MODE_SET(1, 3, PORT_MODE_PP); P13 = 0; // WIFI_UDDATE
	PORT_MODE_SET(1, 4, PORT_MODE_PP); P14 = 0; // WIFI_EN
	PORT_MODE_SET(1, 5, PORT_MODE_PP); P15 = 0;
	PORT_MODE_SET(1, 6, PORT_MODE_PP); P16 = 0; 
	PORT_MODE_SET(1, 7, PORT_MODE_PP); P17 = 0;
	PORT_MODE_SET(2, 0, PORT_MODE_PP); P20 = 0;  // AIR_POWERKEY（拉低1.5S以上）
	PORT_MODE_SET(2, 1, PORT_MODE_PP); P21 = 1;  // AIR_C
	PORT_MODE_SET(2, 2, PORT_MODE_PP); P22 = 0;
	PORT_MODE_SET(2, 3, PORT_MODE_PP); P23 = 0;
	PORT_MODE_SET(2, 4, PORT_MODE_PP); P24 = 0;
	PORT_MODE_SET(2, 5, PORT_MODE_PP); P25 = 0;
	PORT_MODE_SET(2, 6, PORT_MODE_PP); P26 = 0;
	PORT_MODE_SET(2, 7, PORT_MODE_PP); P27 = 0;
	PORT_MODE_SET(3, 0, PORT_MODE_PP); P30 = 0; // 串口1
	PORT_MODE_SET(3, 1, PORT_MODE_PP); P31 = 0; // 串口1
	PORT_MODE_SET(3, 2, PORT_MODE_PP); P32 = 0; // 水银1
	PORT_MODE_SET(3, 3, PORT_MODE_PP); P33 = 0; // 水银2
	PORT_MODE_SET(3, 4, PORT_MODE_PP); P34 = 0;
	PORT_MODE_SET(3, 5, PORT_MODE_PP); P35 = 0;
	PORT_MODE_SET(3, 6, PORT_MODE_PP); P36 = 0; // 映射串口1
	PORT_MODE_SET(3, 7, PORT_MODE_PP); P37 = 0; // 映射串口1
	PORT_MODE_SET(5, 4, PORT_MODE_PP); P54 = 0;
	PORT_MODE_SET(5, 5, PORT_MODE_PP); P55 = 0; 
}

/* 空调GPS滤波窗口值, 不可超过6 */
#define AIR_WIN_CNT		3


/**
  * ***********************************************************************
  * @brief	休眠任务
  *
  *	@param  :
  *
  * @retval void:
  *
  * @confirm	:
  * @attention	: none
  * ***********************************************************************
  */

/* 单片机的睡眠  这是手动设置睡眠时间（掉电定时器一起配套）只有一种情况设定时间睡眠 */
void p_appc_task_sleep_proc()
{
	static int32_t wake_up_remain_time = 0;

	wake_up_remain_time = 0;

	if (g_config.m_fast_sleep == TRUE)
	{
		p_appc_all_deinit();   // 返初始化

		/* 掉电模式时使用内部SCC */
		VOCTRL = 0x00;

		/* 使能掉电定时器 */
		// 1024 0x752f 15s
		WKTCL = 0x2f;
		WKTCH = 0xf5;

		
		/* 设定自己需要醒来的时间（设置定时器的话  就将外部关闭  否则就是设置为外部唤醒）*/
		if (g_config.m_bypass_hour)
		{
			/*
				剩下的睡眠时间(每次15s) = 小时(x)*分钟(60)*秒(60)/周期(15s);
			*/
			//wake_up_remain_time = g_config.m_bypass_hour * 60 * 60 / 15;
			wake_up_remain_time = g_config.m_bypass_hour * 1 * 60 / 15;
			
		}
		/*  上面的寄存器的配置固定的   至于时间的设定看情况 */
		do
		{

			/* 睡觉 每次15秒 进行睡眠的使能(开始循环睡眠)  */
			MCU_SLEEP();

			/* 获取当前醒过来毫秒数（这是前面设定的测试这次睡眠的总时长） */
			--wake_up_remain_time;

			if (wake_up_remain_time <= 0)
			{
				/* 判断是否是旁路唤醒 */
				if (g_config.m_bypass_hour)
				{
					g_config.m_bypass_hour = 0;
					break;
				}
			}

		} while (1);
		p_appc_all_init();     // 正初始化
		//P55 = 1;
	}
	g_config.m_fast_sleep = FALSE;
}

/**
  * ***********************************************************************
  * @brief	向Luat发送设备状态
  *
  *	@param  :
  *
  * @retval void:
  *
  * @confirm	:
  * @attention	: none
  * ***********************************************************************
  */
void p_proc_report_luat(void)
{
	uint8_t i = 0;

	/* 震动次数 */
	CUSTOM_ITOA_3(g_sy1_shark_cnt, g_pkg_to_luat.S1_Data); // 转化为字符存入数组
	CUSTOM_ITOA_3(g_sy2_shark_cnt, g_pkg_to_luat.S2_Data);
	g_sy1_shark_cnt = 0;
	g_sy2_shark_cnt = 0;
	p_uart1_send_data((char*)&g_pkg_to_luat, sizeof(g_pkg_to_luat));
	/* WIFI返回的数据作为另一次发送 */
	if (COM2.RX_Cnt)
	{
		/* 通过如果遇到双引号则在前面多发送过一次反斜杠 */
		p_uart1_send_data_backslash((char*)RX2_Buffer, COM2.RX_Cnt); //发送URST1接收缓冲区
	}
	p_uart1_send_string("\"}");
}


/* 打包的固定格式（发送给LUAT） */
void p_pkg_data_init(void)
{
	g_pkg_to_luat.start = '{';
	strcpy(g_pkg_to_luat.S1, "\"S1\":\"");
	strcpy(g_pkg_to_luat.S2, "\",\"S2\":\"");
	strcpy(g_pkg_to_luat.AT, "\",\"AT\":\"");

}

/**
  * ***********************************************************************
  * @brief	重启Air模块    重启WIFI模块
  *
  *	@param  :
  *
  * @retval void:
  *
  * @confirm	:
  * @attention	: none
  * ***********************************************************************
  */
#if 0
/* 重新启动GSM */
void p_proc_reboot_air(void)
{
	AIR_POWERKEY = 1;
	AIR_POWER_CTRL = 0;
	p_delay_10ms(400);
	AIR_POWERKEY = 0;
	AIR_POWER_CTRL = 1;
}

/* 重新启动WIFI */
void p_proc_rebort_wifi(void)
{
	WIFI_RST = 0;   // 对应的WIFIi的IO口
	WIFI_PU = 0;
	p_delay_10ms(200);
	WIFI_RST = 1;
	WIFI_PU = 1;
}
/* 关闭WIFI和GSM  */
/*  正常的打开GSM和WIFI（放在正初始化函数）有几个问题 */
WIFI_RST = 0;  // 具体对应WIFI哪几个口
WIFI_SET = 1;
WIFI_PU = 0;

AIR_POWERKEY = 0;
AIR_POWER_CTRL = 1;
//GPIO_C_SPI = 1; // ？？？
#endif

/* 执行特殊数据解析和打包的函数  */
uint8_t p_proc_get_lora_data_lengh(uint8_t* p_to_lora_data)
{
	static idata uint8_t cnt = 0;
	cnt = 0;

	while (*p_to_lora_data != '\"')
	{
		++p_to_lora_data;
		++cnt;
	}

	return cnt;
}

uint8_t p_proc_get_lora_data_lengh_skip_space(uint8_t* p_to_lora_data)
{
	static idata uint8_t cnt = 0;
	cnt = 0;

	while (*p_to_lora_data != '\"'
		&& *p_to_lora_data != ' ')
	{
		++p_to_lora_data;
		++cnt;
	}

	return cnt;
}
/***************************************************************************************/
/**
  * ***********************************************************************
  * @brief	解析来自Air的json数据
  *
  *	@param  :
  *
  * @retval void:
  *
  * @confirm	:
  * @attention	: none
  * ***********************************************************************
  */

void p_proc_data_parser_from_air(void)
{
	idata uint8_t to_lora_data_length = 0;
	idata uint8_t index = 0;

	/* 查找包头 */
	while (RX1_Buffer[index] && index < COM1.RX_Cnt)
	{
		if (RX1_Buffer[index++] == '{')
		{

			break;
		}
	}

	/* 判断长度 */
	for (; COM1.RX_Cnt - index >= sizeof(st_item_from_luat); )
	{
		g_p_item_from_luat = (st_item_from_luat*)(RX1_Buffer + index);

		/* 合法性检查 */
		if ('\"' == g_p_item_from_luat->key_name[0]
			&& '\"' == g_p_item_from_luat->key_name[3]
			&& ':' == g_p_item_from_luat->key_name[4])
		{
			/* 数据合法，解析 */
			if (COMPARE_KEY(g_p_item_from_luat->key_name + 1, "ST")
				&& CHECK_VALUE(g_p_item_from_luat->val))
			{
				/* 睡眠时间 */
				g_config.m_bypass_hour = CUSTOM_ATOI_3(g_p_item_from_luat->val);
				index += sizeof(st_item_from_luat);

			}
			else if (COMPARE_KEY(g_p_item_from_luat->key_name + 1, "SD")
				&& CHECK_BOOL(g_p_item_from_luat->val))
			{
				/* 关机使能 */
				g_config.m_fast_sleep = CUSTOM_ATOI_3(g_p_item_from_luat->val);
				if (g_config.m_fast_sleep)
				{
					p_envt_set_event(EVNT_SLEEP);
				}
				index += sizeof(st_item_from_luat);

			}
			else if (COMPARE_KEY(g_p_item_from_luat->key_name + 1, "W1")
				&& CHECK_BOOL(g_p_item_from_luat->val))
			{
				/* 水印开关一 使能 */
				p_envt_set_event(EVNT_WATERMARK);
				g_config.m_enbale_sy1 = CUSTOM_ATOI_3(g_p_item_from_luat->val);
				index += sizeof(st_item_from_luat);

			}
			else if (COMPARE_KEY(g_p_item_from_luat->key_name + 1, "W2")
				&& CHECK_BOOL(g_p_item_from_luat->val))
			{
				/* 水印开关二 使能 */
				p_envt_set_event(EVNT_WATERMARK);
				g_config.m_enbale_sy2 = CUSTOM_ATOI_3(g_p_item_from_luat->val);
				index += sizeof(st_item_from_luat);

			}
			else if (COMPARE_KEY(g_p_item_from_luat->key_name + 1, "AT"))
			{
				to_lora_data_length = p_proc_get_lora_data_lengh_skip_space(g_p_item_from_luat->val);

				if (to_lora_data_length > 0)
				{
					p_uart2_send_data(g_p_item_from_luat->val, to_lora_data_length);
					p_uart2_send_string("\r\n");
					index += p_proc_get_lora_data_lengh(g_p_item_from_luat->val);
					index += 8; // 跳过   ",

				}
				else
				{
					++index;
				}

			}

			else {
				/* 跳过 已经成功判断的 "XX": 共5个字节*/
				index += 5;
			}
		}
		else {
			++index;
		}
	}

}
/**
  * ***********************************************************************
  * @brief	发送数据给AIR （包括WIFI和MCU）
  *
  *	@param  :
  *
  * @retval void:
  *
  * @confirm	:
  * @attention	: none
  * ***********************************************************************
  */
  //void p_proc_data_parser_from_MCU(void)
void p_proc_data_parser_from_loRa(void)
{
	// TODO: WiFi返回数据，转发

	/* 阻塞方式(不用串口中断发送)，发送数据给Air800E */
	/* 构建数据包，再发送 */
	/* 清空，开锁 */
	p_proc_report_luat();

}

#if 0
/* 用于解析和打包的数据类型（仅作参考）  */
/* pkg from luat */

{
	"ST":"002",				// sleep time 睡眠时间（单位 小时）
		"SD" : "001",				// shutdown 关机使能
		"W1" : "001",				// watermark 水印开关1使能
		"W2" : "001",				// watermark 水印开关2使能
		"SH" : "004",				// shake threshod 振动次数报警阈值
		"BT" : "005",				// bypass time 旁路时间（单位 小时）
		"RB" : "000",				// reboot 是否需要重启模块
		"LR" : "xxxxxx"			// LoRa 需要LoRa转发的数据
}


/* 协议格式：*/
{
	"WS":"nor",				// wake source 唤醒源
		"S1" : "004",             // 水印开关2 振动次数
		"AT" : "xxxxxx"           // AT指令返回数据   （WiFi版本才有）
}

/* 唤醒源（设置唤醒数据）是否需要待定 */
	/*
	if (NULL == g_p_boot_reason)
	{

		g_pkg_to_luat.WS_Data[0] = 't';
		g_pkg_to_luat.WS_Data[1] = 'i';
		g_pkg_to_luat.WS_Data[2] = 'm';
	}
	else {
		g_pkg_to_luat.WS_Data[0] = g_p_boot_reason[0];
		g_pkg_to_luat.WS_Data[1] = g_p_boot_reason[1];
		g_pkg_to_luat.WS_Data[2] = g_p_boot_reason[2];
	}
	*/


#endif

