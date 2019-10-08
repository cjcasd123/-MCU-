#ifndef __MAINPROC_H__
#define __MAINPROC_H__

#include "stc8.h"
#include "type.h"
#include "watermark.h"
#include "uart.h"
#include "event.h"
#include "timer.h"
#include "wdg.h"

#define p_appc_uart_send			p_uart2_send_data


#define	abs(val)	((val >= 0) ? val : -val)
#define	RADIX		"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"


#define CUSTOM_ITOA_3(val, p) do {\
									(p)[2] = ((val) % 10)+'0'; (val) /= 10; \
									(p)[1] = ((val) % 10)+'0'; (val) /= 10; (p)[0] = ((val) % 10)+'0';\
								} while (0)

#define CUSTOM_ATOI_3(p)	(((p)[2]-'0')+((p)[1]-'0')*10+((p)[0]-'0')*100)

#define CHECK_VALUE(p)		((p)[0]>='0' && (p)[0]<='9' \
							&& (p)[1]>='0' && (p)[1]<='9' \
							&& (p)[2]>='0' && (p)[2]<='9')

#define CHECK_BOOL(p)		((p)[0]=='0' && (p)[1]=='0' \
							&& ((p)[2]=='0' || (p)[2] == '1'))
#define COMPARE_KEY(src, dst)	((src)[0] == (dst)[0] && (src)[1] == (dst)[1])


#define MCU_SLEEP()	do {_nop_(); _nop_(); PCON |= PD; _nop_(); _nop_(); } while (0)


void p_appc_all_init(void);
void p_pkg_data_init(void);
void p_appc_task_sleep_proc(void);
//void p_proc_reboot_air(void);
void p_proc_report_luat(void);
void p_proc_data_parser_from_air(void);
void p_proc_data_parser_from_loRa(void);
#endif 


