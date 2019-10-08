#ifndef __UART_H__
#define __UART_H__
#include "type.h"

#define enableUart2SendInterrupt		0
#define enableUart1SendInterrupt		1

#define enableUartInterrupts()		ES = 1
#define disableUartInterrupts()		ES = 0


#define enableUart2Interrupts()		IE2 = 1;
#define disableUar2tInterrupts()	IE2 = 0;


#define FOSC  12000000
#define BAUD  115200

void p_uart1_init(void);
//void p_uart1_uninit(void);
void p_uart1_send_byte(char dat);
void p_uart1_send_data(char *p, uint16_t length);
void p_uart1_send_string(char *p);

void p_uart2_init(void);
//void p_uart2_uninit(void);
void p_uart2_send_byte(char dat);
void p_uart2_send_data(char *p, uint8_t length);
void p_uart1_send_data_backslash(char* p, uint16_t length);
void p_uart2_send_string(char *p);

#endif 


