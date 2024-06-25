#ifndef ONLINE_CONFIG_H
#define ONLINE_CONFIG_H


#define CI_UART         0
#define CI_TWS          1

void ci_data_rx_handler(u8 type);
u32 eq_cfg_sync(u8 priority);
void ci_uart_write(char *buf, u16 len);
#endif

