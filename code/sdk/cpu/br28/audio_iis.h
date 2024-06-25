#ifndef __AUDIO_IIS_H
#define __AUDIO_IIS_H

#include <cpu.h>

void alink_iis_init(int dma_size, int sr);
void alink_iis_start(void *rx_priv, void *tx_priv, void (*rx_handle)(void *priv, void *addr, int len), void (*tx_handle)(void *priv, void *addr, int len));
void alink_rx_set_shn(int len);
int audio_iis_output_write(s16 *data, u32 len);




#endif
