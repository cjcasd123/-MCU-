/**
  *************************************************************************
  * @file    event.h
  * @author  monkey.Huaqian
  * @version V1.0.0
  * @date    2019/03/08 15:17:48
  * @brief
  *************************************************************************
  * @attention 最大支持32个事件
  *
  * <h2><center>&Copyright(c) 2014-2020 Xiamen HuaQian Studio</center></h2>
  *
  *************************************************************************
  */

  /*************************     防重复包含定义     *************************/
#ifndef EVENT_H
#define EVENT_H

/*************************     系统头文件包含     *************************/

/*************************     私有头文件包含     *************************/
#include "type.h"

/*************************      导入命名空间      *************************/

/*************************      C++ 兼容定义      *************************/
#ifdef __cplusplus 
#if __cplusplus 
extern "C" {
#endif 
#endif

	/*************************        全局枚举        *************************/
	typedef enum
	{
		EVNT_INIT,					// 系统初始化 不处理
		/* 事件添加 begin */
		EVNT_WATERMARK,			    // 水印震动判断（开启水银震动）
		EVNT_SLEEP,					// 睡眠任务
		/* 事件添加 end */
		EVNT_MAX					// 事件最大序号，压在最后
	}EVNT_Handle;

	/*************************       全局宏定义       *************************/



	/*************************     全局结构体定义     *************************/

#define p_evnt_init()					g_evnt_triggle_flg = 0      // 标志各个事件是否触发（改变8位数据中的一位）

#define p_envt_set_event(event)			_envt_set_event(event)		// 设置事件触发
#define _envt_set_event(event)			do{ g_evnt_triggle_flg |= (0x01<<(uint8_t)(event));}while(0)

#define p_envt_clear_event(event)		_envt_clear_event(event)	// 清楚事件触发标志
#define _envt_clear_event(event)		do{ g_evnt_triggle_flg &= ~(0x01<<(uint8_t)(event));}while(0)

#define p_envt_get_event(event)			_envt_get_event(event)		// 判断事件触发标志
#define _envt_get_event(event)			(uint8_t)((g_evnt_triggle_flg & (0x01<<(uint8_t)(event)))!=0)

#define p_evnt_clear()					g_evnt_triggle_flg = 0;

/*************************      全局变量定义      *************************/
	extern uint32_t  g_evnt_triggle_flg;

	/*************************        函数声明        *************************/

	/*************************      C++ 兼容定义      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif

/*************************     防重复包含定义     *************************/
#endif



