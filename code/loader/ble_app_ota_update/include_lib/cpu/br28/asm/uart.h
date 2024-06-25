#ifndef __UART_H__
#define __UART_H__

#include "common.h"


void mask_init(void *exp_hook, void *pchar);

// void mask_init(void *f, void *m, void *stack_free, void *stack_malloc,
//                void *suppress_tick_and_sleep,
//                void *StartFirstTask,
//                void *exp_hook, void *i_hook,
//                void *pchar, void *log_e);

void uart_init(const char *tx_io, u32 baud);
void putchar(char a);
void uart_close(void);

void ut_device_mode(const char *ut_tx, const char *ut_rx, u32 baud);


#endif
