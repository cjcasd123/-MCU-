/**
  *************************************************************************
  * @file    config.h
  * @author  Toson.Huaqian
  * @version V1.0.0
  * @date    2018/11/12 18:10:43
  * @brief
  *************************************************************************
  * @attention
  *
  * <h2><center>&Copyright(c) 2014-2020 Xiamen HuaQian Studio</center></h2>
  *
  *************************************************************************
  */

  /*************************     防重复包含定义     *************************/
#ifndef CONFIG_H
#define CONFIG_H

/*************************     系统头文件包含     *************************/
#include "stc8.h"
#include "type.h"

/*************************     私有头文件包含     *************************/

/*************************      导入命名空间      *************************/

/*************************      C++ 兼容定义      *************************/
#ifdef __cplusplus 
#if __cplusplus 
extern "C" {
#endif 
#endif

	/*************************        全局枚举        *************************/

	/*************************       全局宏定义       *************************/

#define enableInterrupts()		EA = 1
#define disableInterrupts()		EA = 0

#define PORT_MODE_AD			0x00	// 准双向扣
#define PORT_MODE_PP			0x01	// 推挽输出
#define PORT_MODE_HZ			0x02	// 高阻输入
#define PORT_MODE_OD			0x03	// 开口输出

/* 设置端口的模式 */
#define PORT_MODE_SET(g, p, m)	_PORT_MODE_SET(g, p, m)
#define _PORT_MODE_SET(g, p, m)	do{\
									P##g##M0 &= ~(0x01<<(p));\
									P##g##M1 &= ~(0x01<<(p));\
									P##g##M0 |= ((m&0x01)<<(p)); \
									P##g##M1 |= ((m>>1)<<(p)); \
								}while(0)

/* 设置端口是否需要上拉电阻3.7K, P3.0 P3.1会小一点 */
#define PORT_PULL_UP_SET(g, p)	_PORT_PULL_UP_SET(g, p)
#define _PORT_PULL_UP_SET(g, p)	do{P##g##PU &= ~(0x01<<(p)); P##g##PU |= (0x01<<(p));}while(0)

/* 准确的内部时钟 */
#define IRC32K_FREP				(uint16_t)(*((uint16_t*)(0xF8)))

/* 水银开关（外部中断） */
#define SY1_G				3
#define SY1_P				2
#define SY1					P32

#define SY2_G				3
#define SY2_P				3
#define SY2					P33

/* WIFI连接的另外三个IO口（复位 下载 使能）  */
#define WIFI_RST_G  1
#define WIFI_RST_P  2
#define WIFI_RST P12

#define WIFI_UPDATE P13
#define WIFI_UPDATE_G 1
#define WIFI_UPDATE_P 3

#define WIFI_EN_G  1
#define WIFI_EN_P  4
#define WIFI_EN P14
/* Air800E模块-GSM/GNSS （断开GSM的电源）*/
#define AIR_POWERKEY_G		2
#define AIR_POWERKEY_P		0
#define AIR_POWERKEY		P20

#define AIR_POWER_CTRL_G		2
#define AIR_POWER_CTRL_P		1
#define AIR_POWER_CTRL			P21

/* 发送到WIFI的串口端口 */
#define WIFI_RX_G			1
#define WIFI_RX_P			0
#define WIFI_RX				P10

#define WIFI_TX_G			1
#define WIFI_TX_P			1
#define WIFI_TX				P11

/* GSM模块（这个后面要改到P36P37） */
#define AIR_RX_G				3
#define AIR_RX_P				0
#define AIR_RX				P30

#define AIR_TX_G				3
#define AIR_TX_P				1
#define AIR_TX				P31

/* 下载代码和固件的IO口 */
#define  IAP_RX    P30
#define  IAP_TX    P31

#define	COM_TX1_Lenth	256
#define	COM_RX1_Lenth	512

#define	COM_TX2_Lenth	128
#define	COM_RX2_Lenth	256	

#define	TimeOutSet1		5
#define	TimeOutSet2		2
#define NULL			(void*)0

#define heart_send    5
#define heart_clear   0
#define heart_time    100

#define WDT_DUAL_CORE_SEC	120
#define my_time()			g_time_ms

/*************************     全局结构体定义     *************************/
	typedef struct
	{
		u8	TX_read;		//发送读指针
		u8	TX_write;		//发送写指针
		u8	B_TX_busy;		//忙标志

		u8	RX_TimeOut;		//接收超时
		u16 RX_Cnt;			//接收字节计数
	} COMx_Define;

	typedef struct
	{
		// 	u8  m_is_config_ok;    // 配置是否可用
		u8  m_enbale_sy1;	   // 使能水印1
		u8  m_enbale_sy2;	   // 使能水印2
		//u8  m_sy_shark_cnt;    // 水银晃动报警次数
		//u8  m_sy2_shark_cnt;   // 水银晃动报警次数
		u8  m_fast_sleep;      // 是否马上睡觉
	//	u16 m_timer_report;    // 定时上报hour, 设定成0则不定时上报
		u16 m_bypass_hour;	   // 旁路时间,设置成0不开启旁路功能
	}st_now_config;

	typedef struct
	{
		char start;
		//char WS[6];
		//char WS_Data[3];
		char S1[6];
		char S1_Data[3];
		char S2[8];
		char S2_Data[3];
		char AT[8];
	}st_pkg_to_luat;

	typedef struct
	{
		char key_name[6];
		char val[5];
	}st_item_from_luat;


	/*************************      全局变量定义      *************************/
	extern idata COMx_Define		COM1;
	extern idata COMx_Define		COM2;
	extern xdata u8			RX1_Buffer[];	// 发送缓冲
	extern xdata u8			TX1_Buffer[];	// 发送缓冲
	extern xdata u8			RX2_Buffer[];	// 发送缓冲
	extern xdata u8			TX2_Buffer[];	// 发送缓冲
	extern st_now_config	g_config;
	//extern idata char*		g_p_boot_reason;

	extern xdata st_pkg_to_luat			g_pkg_to_luat;
	extern idata st_item_from_luat*		g_p_item_from_luat;

	extern u32			g_time_ms;
	extern u8			g_sy1_shark_cnt;
	extern u8			g_sy2_shark_cnt;
	extern u8           g_heart_beat, g_heart_sign;
	//extern u32			next_close_bypass_ts;

	/*************************      C++ 兼容定义      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif

/*************************     防重复包含定义     *************************/
#endif