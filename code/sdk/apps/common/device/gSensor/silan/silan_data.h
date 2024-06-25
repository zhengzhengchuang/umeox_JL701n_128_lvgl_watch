#pragma once

#include "app_config.h"

unsigned char sl_i2c_write(unsigned char reg, unsigned char dat);
unsigned char sl_i2c_read(unsigned char reg, unsigned char len, unsigned char *buf);

void sl_algo_input(short *data, short point);
unsigned char sl_fifo_length(void);

void sl_data_debug(char enable);