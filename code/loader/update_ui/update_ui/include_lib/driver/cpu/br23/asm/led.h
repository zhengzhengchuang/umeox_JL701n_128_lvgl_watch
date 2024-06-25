#ifndef _LED_H_
#define _LED_H_



#include "typedef.h"

void rgb_led_send_data(u8 *data, u32 len);
void rgb_led_spi_init(void);
void rgb_led_scan(void);
void rgb_led_disable(void);

#endif
